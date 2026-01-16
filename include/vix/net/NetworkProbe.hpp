/**
 *
 *  @file NetworkProbe.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.  All rights reserved.
 *  https://github.com/vixcpp/vix
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 */
#ifndef VIX_NETWORK_PROBE_HPP
#define VIX_NETWORK_PROBE_HPP

#include <cstdint>
#include <functional>

namespace vix::net
{

  class NetworkProbe
  {
  public:
    using ProbeFn = std::function<bool()>; // returns true if online

    struct Config
    {
      std::int64_t min_interval_ms{1000};
      std::int64_t online_ttl_ms{2000}; // cached online validity
      std::int64_t offline_ttl_ms{500}; // cached offline validity
    };

    NetworkProbe(Config cfg, ProbeFn fn);

    bool isOnline(std::int64_t now_ms) const;
    bool refresh(std::int64_t now_ms);
    bool lastKnownOnline() const noexcept { return last_online_; }
    std::int64_t lastProbeAtMs() const noexcept { return last_probe_at_ms_; }

  private:
    bool canProbe(std::int64_t now_ms) const noexcept;

  private:
    Config cfg_;
    ProbeFn probe_;

    bool last_online_{false};
    std::int64_t last_probe_at_ms_{0};
    std::int64_t last_update_ms_{0};
  };

} // namespace vix::net

#endif
