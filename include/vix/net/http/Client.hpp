/**
 *
 *  @file Client.hpp
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
#ifndef VIX_NET_HTTP_CLIENT_HPP
#define VIX_NET_HTTP_CLIENT_HPP

#include <vix/error/Result.hpp>
#include <vix/net/http/ClientRequest.hpp>
#include <vix/net/http/ClientResponse.hpp>

namespace vix::net::http
{
  /**
   * @brief Standard result type for HTTP client operations.
   */
  using ClientResult = vix::error::Result<ClientResponse>;

  /**
   * @class Client
   * @brief Abstract outbound HTTP client interface.
   *
   * Client is the provider-neutral HTTP abstraction used by Vix modules that
   * need to send outbound HTTP requests without depending on a concrete backend.
   */
  class Client
  {
  public:
    /**
     * @brief Construct an HTTP client.
     */
    Client() noexcept = default;

    /**
     * @brief Destroy the HTTP client.
     */
    virtual ~Client() noexcept = default;

    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    Client(Client &&) noexcept = delete;
    Client &operator=(Client &&) noexcept = delete;

    /**
     * @brief Send an outbound HTTP request.
     *
     * @param request HTTP client request description.
     * @return ClientResponse on success, structured error on backend failure.
     */
    [[nodiscard]] virtual ClientResult send(
        const ClientRequest &request) const = 0;
  };

} // namespace vix::net::http

#endif // VIX_NET_HTTP_CLIENT_HPP
