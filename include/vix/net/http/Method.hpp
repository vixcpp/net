/**
 *
 *  @file Method.hpp
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
#ifndef VIX_NET_HTTP_METHOD_HPP
#define VIX_NET_HTTP_METHOD_HPP

#include <string_view>

namespace vix::net::http
{
  /**
   * @enum Method
   * @brief HTTP methods supported by the Vix HTTP client layer.
   *
   * This enum represents outbound HTTP request methods.
   */
  enum class Method
  {
    Get,
    Post,
    Put,
    Patch,
    Delete,
    Head,
    Options
  };

  /**
   * @brief Convert an HTTP method to its wire string representation.
   *
   * @param method HTTP method.
   * @return Uppercase HTTP method name.
   */
  [[nodiscard]] inline constexpr std::string_view to_string(Method method) noexcept
  {
    switch (method)
    {
    case Method::Get:
      return "GET";
    case Method::Post:
      return "POST";
    case Method::Put:
      return "PUT";
    case Method::Patch:
      return "PATCH";
    case Method::Delete:
      return "DELETE";
    case Method::Head:
      return "HEAD";
    case Method::Options:
      return "OPTIONS";
    }

    return "GET";
  }

} // namespace vix::net::http

#endif // VIX_NET_HTTP_METHOD_HPP
