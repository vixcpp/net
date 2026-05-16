/**
 *
 *  @file ClientRequest.hpp
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
#ifndef VIX_NET_HTTP_CLIENTREQUEST_HPP
#define VIX_NET_HTTP_CLIENTREQUEST_HPP

#include <cstdint>
#include <string>
#include <utility>

#include <vix/net/http/HeaderMap.hpp>
#include <vix/net/http/Method.hpp>

namespace vix::net::http
{
  /**
   * @struct ClientRequest
   * @brief Outbound HTTP request description for the Vix HTTP client layer.
   */
  struct ClientRequest
  {
    /**
     * @brief HTTP method used for the request.
     */
    Method method{Method::Get};

    /**
     * @brief Absolute request URL.
     *
     * Example:
     * http://127.0.0.1:11434/api/generate
     */
    std::string url{};

    /**
     * @brief HTTP request headers.
     */
    HeaderMap headers{};

    /**
     * @brief HTTP request body.
     */
    std::string body{};

    /**
     * @brief Request timeout in milliseconds.
     *
     * 0 means no explicit timeout.
     */
    std::uint64_t timeout_ms{0};

    /**
     * @brief Return true if the request has a usable URL.
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return !url.empty();
    }

    /**
     * @brief Set the HTTP method.
     */
    ClientRequest &set_method(Method value) noexcept
    {
      method = value;
      return *this;
    }

    /**
     * @brief Set the request URL.
     */
    ClientRequest &set_url(std::string value)
    {
      url = std::move(value);
      return *this;
    }

    /**
     * @brief Set the request body.
     */
    ClientRequest &set_body(std::string value)
    {
      body = std::move(value);
      return *this;
    }

    /**
     * @brief Set request timeout in milliseconds.
     */
    ClientRequest &set_timeout_ms(std::uint64_t value) noexcept
    {
      timeout_ms = value;
      return *this;
    }

    /**
     * @brief Set or replace one request header.
     */
    ClientRequest &set_header(std::string name, std::string value)
    {
      headers[std::move(name)] = std::move(value);
      return *this;
    }
  };

} // namespace vix::net::http

#endif // VIX_NET_HTTP_CLIENTREQUEST_HPP
