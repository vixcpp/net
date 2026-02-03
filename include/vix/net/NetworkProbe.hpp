/**
 *
 *  @file NetworkProbe.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
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
  /**
   * @brief Lightweight network connectivity probe with caching.
   *
   * NetworkProbe encapsulates a user-provided probe function and adds:
   * - rate limiting (minimum probe interval)
   * - cached online/offline state with separate TTLs
   * - monotonic-time based decisions
   *
   * It is designed to be used by offline-first systems to avoid excessive
   * network checks while still reacting quickly to connectivity changes.
   */
  class NetworkProbe
  {
  public:
    /**
     * @brief Probe function type.
     *
     * The function should return true if the network is considered online,
     * false otherwise.
     */
    using ProbeFn = std::function<bool()>; // returns true if online

    /**
     * @brief Configuration for the network probe.
     */
    struct Config
    {
      /**
       * @brief Minimum interval between two actual probe executions.
       */
      std::int64_t min_interval_ms{1000};

      /**
       * @brief Time-to-live for a cached "online" result.
       */
      std::int64_t online_ttl_ms{2000};

      /**
       * @brief Time-to-live for a cached "offline" result.
       */
      std::int64_t offline_ttl_ms{500};
    };

    /**
     * @brief Construct a NetworkProbe.
     *
     * @param cfg Probe configuration.
     * @param fn Probe function invoked to test connectivity.
     */
    NetworkProbe(Config cfg, ProbeFn fn);

    /**
     * @brief Check whether the network is currently considered online.
     *
     * This method may return a cached result if the TTL has not expired.
     *
     * @param now_ms Current time in milliseconds.
     * @return true if online, false otherwise.
     */
    bool isOnline(std::int64_t now_ms) const;

    /**
     * @brief Force a probe refresh if allowed by rate limiting.
     *
     * @param now_ms Current time in milliseconds.
     * @return Latest online state.
     */
    bool refresh(std::int64_t now_ms);

    /**
     * @brief Return the last known online state.
     *
     * This does not trigger a probe.
     */
    bool lastKnownOnline() const noexcept { return last_online_; }

    /**
     * @brief Timestamp of the last probe attempt.
     */
    std::int64_t lastProbeAtMs() const noexcept { return last_probe_at_ms_; }

  private:
    /**
     * @brief Check whether a new probe is allowed at the given time.
     */
    bool canProbe(std::int64_t now_ms) const noexcept;

  private:
    /**
     * @brief Stored probe configuration.
     */
    Config cfg_;

    /**
     * @brief User-provided probe function.
     */
    ProbeFn probe_;

    /**
     * @brief Last known online state.
     */
    bool last_online_{false};

    /**
     * @brief Timestamp of the last probe execution.
     */
    std::int64_t last_probe_at_ms_{0};

    /**
     * @brief Timestamp of the last cached state update.
     */
    std::int64_t last_update_ms_{0};
  };

} // namespace vix::net

#endif // VIX_NETWORK_PROBE_HPP
