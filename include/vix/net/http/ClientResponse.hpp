/**
 *
 *  @file ClientResponse.hpp
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
#ifndef VIX_NET_HTTP_CLIENTRESPONSE_HPP
#define VIX_NET_HTTP_CLIENTRESPONSE_HPP

#include <string>
#include <string_view>

#include <vix/net/http/HeaderMap.hpp>

namespace vix::net::http
{
  /**
   * @struct ClientResponse
   * @brief Inbound HTTP response returned by the Vix HTTP client layer.
   */
  struct ClientResponse
  {
    /**
     * @brief Numeric HTTP status code.
     *
     * 0 means the backend could not determine the HTTP status code.
     */
    int status_code{0};

    /**
     * @brief Response headers.
     */
    HeaderMap headers{};

    /**
     * @brief Response body.
     */
    std::string body{};

    /**
     * @brief Raw backend error text when available.
     */
    std::string error{};

    /**
     * @brief Return true if the status code is a 2xx success code.
     */
    [[nodiscard]] bool success() const noexcept
    {
      return status_code >= 200 && status_code <= 299;
    }

    /**
     * @brief Return true if the response contains an error message.
     */
    [[nodiscard]] bool has_error() const noexcept
    {
      return !error.empty();
    }

    /**
     * @brief Return true if the response has a status code.
     */
    [[nodiscard]] bool has_status_code() const noexcept
    {
      return status_code > 0;
    }

    /**
     * @brief Return a header value or an empty view if missing.
     */
    [[nodiscard]] std::string_view header(std::string_view name) const noexcept
    {
      auto it = headers.find(std::string(name));
      if (it == headers.end())
      {
        return {};
      }

      return it->second;
    }
  };

} // namespace vix::net::http

#endif // VIX_NET_HTTP_CLIENTRESPONSE_HPP
