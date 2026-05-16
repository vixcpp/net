/**
 *
 *  @file HeaderMap.hpp
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
#ifndef VIX_NET_HTTP_HEADERMAP_HPP
#define VIX_NET_HTTP_HEADERMAP_HPP

#include <string>
#include <unordered_map>

namespace vix::net::http
{
  /**
   * @brief HTTP header container used by the Vix HTTP client layer.
   *
   * HeaderMap stores outbound request headers and inbound response headers.
   *
   * Keys are stored as provided by the caller. The client implementation is
   * responsible for serializing them to the wire format.
   */
  using HeaderMap = std::unordered_map<std::string, std::string>;

} // namespace vix::net::http

#endif // VIX_NET_HTTP_HEADERMAP_HPP
