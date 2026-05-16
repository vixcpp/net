/**
 *
 *  @file NetworkProbeTest.cpp
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

#include <cassert>
#include <vix/net/NetworkProbe.hpp>

int main()
{
  {
    vix::net::NetworkProbe probe;

    assert(!probe.last_known_online());
    assert(!probe.cache_valid(0));

    const bool online = probe.is_online(1000);

    assert(!online);
    assert(probe.cache_valid(1000));
    assert(probe.last_probe_at_ms() == 1000);
    assert(probe.last_update_ms() == 1000);
  }

  {
    bool called = false;

    vix::net::NetworkProbe probe(
        vix::net::NetworkProbe::Config{},
        [&called]()
        {
          called = true;
          return true;
        });

    assert(probe.is_online(1000));
    assert(called);
    assert(probe.last_known_online());
  }

  {
    int calls = 0;

    vix::net::NetworkProbe::Config cfg;
    cfg.min_interval_ms = 1000;
    cfg.online_ttl_ms = 2000;
    cfg.offline_ttl_ms = 500;

    vix::net::NetworkProbe probe(
        cfg,
        [&calls]()
        {
          ++calls;
          return calls >= 2;
        });

    assert(!probe.refresh(1000));
    assert(calls == 1);

    assert(!probe.refresh(1200));
    assert(calls == 1);

    assert(probe.refresh(2200));
    assert(calls == 2);
  }

  {
    vix::net::NetworkProbe::Config cfg;
    cfg.fallback_online = true;

    vix::net::NetworkProbe probe(cfg, {});

    assert(probe.is_online(1000));
    assert(probe.last_known_online());
  }

  return 0;
}
