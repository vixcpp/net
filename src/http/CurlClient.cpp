/**
 *
 *  @file CurlClient.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 *
 */

#include <vix/net/http/CurlClient.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <vix/net/http/ClientError.hpp>
#include <vix/net/http/Method.hpp>
#include <vix/process/Command.hpp>
#include <vix/process/Output.hpp>
#include <vix/process/PipeMode.hpp>

namespace vix::net::http
{
  namespace
  {
    [[nodiscard]] bool starts_with_http_endpoint(
        std::string_view url) noexcept
    {
      return url.rfind("http://", 0) == 0 ||
             url.rfind("https://", 0) == 0;
    }

    [[nodiscard]] std::uint64_t timeout_seconds(
        std::uint64_t timeout_ms) noexcept
    {
      if (timeout_ms == 0)
      {
        return 0;
      }

      const std::uint64_t seconds = timeout_ms / 1000;
      return seconds == 0 ? 1 : seconds;
    }

    [[nodiscard]] std::string trim_copy(std::string_view value)
    {
      std::string out(value);

      auto not_space = [](unsigned char ch)
      {
        return !std::isspace(ch);
      };

      out.erase(
          out.begin(),
          std::find_if(out.begin(), out.end(), not_space));

      out.erase(
          std::find_if(out.rbegin(), out.rend(), not_space).base(),
          out.end());

      return out;
    }

    [[nodiscard]] int parse_status_code(std::string_view value) noexcept
    {
      const std::string text = trim_copy(value);

      if (text.empty())
      {
        return 0;
      }

      int status = 0;

      for (const char ch : text)
      {
        if (!std::isdigit(static_cast<unsigned char>(ch)))
        {
          return 0;
        }

        status = (status * 10) + (ch - '0');
      }

      return status;
    }

    [[nodiscard]] std::string curl_method_argument(Method method)
    {
      return std::string(to_string(method));
    }

    void append_headers(
        std::vector<std::string> &args,
        const HeaderMap &headers)
    {
      for (const auto &[name, value] : headers)
      {
        args.push_back("-H");
        args.push_back(name + ": " + value);
      }
    }

    void append_timeout(
        std::vector<std::string> &args,
        std::uint64_t timeout_ms)
    {
      const std::uint64_t seconds = timeout_seconds(timeout_ms);

      if (seconds == 0)
      {
        return;
      }

      args.push_back("--max-time");
      args.push_back(std::to_string(seconds));
    }

    [[nodiscard]] std::vector<std::string> build_curl_args(
        const ClientRequest &request)
    {
      std::vector<std::string> args;

      args.push_back("-sS");
      args.push_back("-w");
      args.push_back("\n%{http_code}");

      append_timeout(args, request.timeout_ms);

      args.push_back("-X");
      args.push_back(curl_method_argument(request.method));

      append_headers(args, request.headers);

      if (!request.body.empty())
      {
        args.push_back("-d");
        args.push_back(request.body);
      }

      args.push_back(request.url);

      return args;
    }

    [[nodiscard]] ClientResponse build_response_from_stdout(
        std::string stdout_text,
        std::string stderr_text)
    {
      ClientResponse response;
      response.error = std::move(stderr_text);

      const auto marker = stdout_text.rfind('\n');
      if (marker == std::string::npos)
      {
        response.body = std::move(stdout_text);
        response.status_code = 0;
        return response;
      }

      const std::string status_text = stdout_text.substr(marker + 1);
      response.status_code = parse_status_code(status_text);
      response.body = stdout_text.substr(0, marker);

      return response;
    }
  } // namespace

  ClientResult CurlClient::send(const ClientRequest &request) const
  {
    if (!request.valid())
    {
      return make_client_error(
          ClientErrorCode::EmptyUrl,
          "HTTP client request URL cannot be empty");
    }

    if (!starts_with_http_endpoint(request.url))
    {
      return make_client_error(
          ClientErrorCode::InvalidUrl,
          "HTTP client request URL must start with http:// or https://");
    }

    vix::process::Command command("curl");

    command.args(build_curl_args(request))
        .stdout_mode(vix::process::PipeMode::Pipe)
        .stderr_mode(vix::process::PipeMode::Pipe)
        .stdin_mode(vix::process::PipeMode::Null)
        .search_in_path(true)
        .detach(false)
        .inherit_environment(true);

    auto output = vix::process::output(command);
    if (!output)
    {
      return make_client_error(
          ClientErrorCode::BackendUnavailable,
          std::string(output.error().message()));
    }

    ClientResponse response = build_response_from_stdout(
        std::move(output.value().stdout_text),
        std::move(output.value().stderr_text));

    if (!output.value().success())
    {
      if (response.error.empty())
      {
        response.error = response.body;
      }

      return response;
    }

    return response;
  }

} // namespace vix::net::http
