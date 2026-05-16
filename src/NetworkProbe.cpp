/**
 *
 *  @file NetworkProbe.cpp
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

#include <vix/net/NetworkProbe.hpp>

#include <utility>

namespace vix::net
{
  NetworkProbe::NetworkProbe(ProbeFn fn)
      : NetworkProbe(Config{}, std::move(fn))
  {
  }

  NetworkProbe::NetworkProbe(Config cfg, ProbeFn fn)
      : cfg_(std::move(cfg)),
        probe_(std::move(fn)),
        last_online_(cfg_.initial_online)
  {
    if (!cfg_.valid())
    {
      cfg_ = Config{};
      last_online_ = cfg_.initial_online;
    }
  }

  bool NetworkProbe::cache_valid(std::int64_t now_ms) const noexcept
  {
    if (!initialized_)
    {
      return false;
    }

    const std::int64_t age_ms = now_ms - last_update_ms_;
    if (age_ms < 0)
    {
      return false;
    }

    const std::int64_t ttl_ms =
        last_online_ ? cfg_.online_ttl_ms : cfg_.offline_ttl_ms;

    return age_ms <= ttl_ms;
  }

  bool NetworkProbe::is_online(std::int64_t now_ms)
  {
    if (cache_valid(now_ms))
    {
      return last_online_;
    }

    return refresh(now_ms);
  }

  bool NetworkProbe::refresh(std::int64_t now_ms)
  {
    if (!can_probe(now_ms))
    {
      return last_online_;
    }

    return run_probe(now_ms);
  }

  bool NetworkProbe::force_refresh(std::int64_t now_ms)
  {
    return run_probe(now_ms);
  }

  void NetworkProbe::set_probe(ProbeFn fn)
  {
    probe_ = std::move(fn);
  }

  void NetworkProbe::set_config(Config cfg)
  {
    if (!cfg.valid())
    {
      cfg = Config{};
    }

    cfg_ = std::move(cfg);

    if (!initialized_)
    {
      last_online_ = cfg_.initial_online;
    }
  }

  bool NetworkProbe::can_probe(std::int64_t now_ms) const noexcept
  {
    if (!initialized_)
    {
      return true;
    }

    const std::int64_t age_ms = now_ms - last_probe_at_ms_;
    if (age_ms < 0)
    {
      return true;
    }

    return age_ms >= cfg_.min_interval_ms;
  }

  bool NetworkProbe::run_probe(std::int64_t now_ms)
  {
    last_probe_at_ms_ = now_ms;

    if (!probe_)
    {
      last_online_ = cfg_.fallback_online;
      last_update_ms_ = now_ms;
      initialized_ = true;
      return last_online_;
    }

    last_online_ = probe_();
    last_update_ms_ = now_ms;
    initialized_ = true;

    return last_online_;
  }

} // namespace vix::net
