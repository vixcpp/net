/**
 *
 *  @file CurlClient.hpp
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
#ifndef VIX_NET_HTTP_CURLCLIENT_HPP
#define VIX_NET_HTTP_CURLCLIENT_HPP

#include <vix/net/http/Client.hpp>

namespace vix::net::http
{
  /**
   * @class CurlClient
   * @brief HTTP client backend implemented with the curl command-line tool.
   *
   * CurlClient is a temporary portable backend for the Vix HTTP client layer.
   * It delegates process execution to vix::process and keeps curl hidden behind
   * the net/http abstraction.
   */
  class CurlClient final : public Client
  {
  public:
    /**
     * @brief Construct a curl-backed HTTP client.
     */
    CurlClient() noexcept = default;

    /**
     * @brief Destroy the curl-backed HTTP client.
     */
    ~CurlClient() noexcept override = default;

    CurlClient(const CurlClient &) = delete;
    CurlClient &operator=(const CurlClient &) = delete;

    CurlClient(CurlClient &&) noexcept = delete;
    CurlClient &operator=(CurlClient &&) noexcept = delete;

    /**
     * @brief Send an outbound HTTP request using curl.
     *
     * @param request HTTP client request description.
     * @return ClientResponse on success, structured error on backend failure.
     */
    [[nodiscard]] ClientResult send(
        const ClientRequest &request) const override;
  };

} // namespace vix::net::http

#endif // VIX_NET_HTTP_CURLCLIENT_HPP
