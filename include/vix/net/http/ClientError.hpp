/**
 *
 *  @file ClientError.hpp
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
#ifndef VIX_NET_HTTP_CLIENTERROR_HPP
#define VIX_NET_HTTP_CLIENTERROR_HPP

#include <string>
#include <utility>

#include <vix/error/Error.hpp>
#include <vix/error/ErrorCategory.hpp>
#include <vix/error/ErrorCode.hpp>

namespace vix::net::http
{
  /**
   * @enum ClientErrorCode
   * @brief HTTP client-specific semantic error codes.
   */
  enum class ClientErrorCode
  {
    None = 0,
    InvalidRequest,
    EmptyUrl,
    InvalidUrl,
    UnsupportedMethod,
    BackendUnavailable,
    RequestFailed,
    Timeout,
    ResponseInvalid,
    UnsupportedOperation
  };

  /**
   * @brief Return the default HTTP client error category.
   */
  [[nodiscard]] inline constexpr vix::error::ErrorCategory client_error_category() noexcept
  {
    return vix::error::ErrorCategory("net.http.client");
  }

  /**
   * @brief Convert a ClientErrorCode to a generic Vix ErrorCode.
   */
  [[nodiscard]] inline constexpr vix::error::ErrorCode to_error_code(
      ClientErrorCode code) noexcept
  {
    using vix::error::ErrorCode;

    switch (code)
    {
    case ClientErrorCode::None:
      return ErrorCode::Ok;

    case ClientErrorCode::InvalidRequest:
    case ClientErrorCode::EmptyUrl:
    case ClientErrorCode::InvalidUrl:
    case ClientErrorCode::UnsupportedMethod:
      return ErrorCode::InvalidArgument;

    case ClientErrorCode::BackendUnavailable:
      return ErrorCode::NotFound;

    case ClientErrorCode::Timeout:
      return ErrorCode::Timeout;

    case ClientErrorCode::UnsupportedOperation:
      return ErrorCode::NotSupported;

    case ClientErrorCode::RequestFailed:
    case ClientErrorCode::ResponseInvalid:
      return ErrorCode::ExternalError;
    }

    return ErrorCode::Unknown;
  }

  /**
   * @brief Convert a ClientErrorCode to a human-readable name.
   */
  [[nodiscard]] inline const char *to_string(ClientErrorCode code) noexcept
  {
    switch (code)
    {
    case ClientErrorCode::None:
      return "none";
    case ClientErrorCode::InvalidRequest:
      return "invalid_request";
    case ClientErrorCode::EmptyUrl:
      return "empty_url";
    case ClientErrorCode::InvalidUrl:
      return "invalid_url";
    case ClientErrorCode::UnsupportedMethod:
      return "unsupported_method";
    case ClientErrorCode::BackendUnavailable:
      return "backend_unavailable";
    case ClientErrorCode::RequestFailed:
      return "request_failed";
    case ClientErrorCode::Timeout:
      return "timeout";
    case ClientErrorCode::ResponseInvalid:
      return "response_invalid";
    case ClientErrorCode::UnsupportedOperation:
      return "unsupported_operation";
    }

    return "unknown";
  }

  /**
   * @brief Build a structured Vix error from a ClientErrorCode.
   *
   * @param code HTTP client-specific error code.
   * @param message Human-readable message.
   */
  [[nodiscard]] inline vix::error::Error make_client_error(
      ClientErrorCode code,
      std::string message)
  {
    return vix::error::Error(
        to_error_code(code),
        client_error_category(),
        std::move(message));
  }

} // namespace vix::net::http

#endif // VIX_NET_HTTP_CLIENTERROR_HPP
