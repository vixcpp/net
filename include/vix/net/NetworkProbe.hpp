/**
 *
 *  @file NetworkProbe.hpp
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
#ifndef VIX_NET_NETWORKPROBE_HPP
#define VIX_NET_NETWORKPROBE_HPP

#include <cstdint>
#include <functional>

namespace vix::net
{
  /**
   * @class NetworkProbe
   * @brief Lightweight network connectivity probe with cached state.
   *
   * NetworkProbe wraps a user-provided connectivity check and adds:
   * - minimum interval between real probe executions
   * - separate TTLs for online and offline cached states
   * - explicit refresh control
   *
   * It is useful for offline-first systems that need to avoid excessive
   * network checks while still keeping a recent connectivity state.
   */
  class NetworkProbe
  {
  public:
    /**
     * @brief Probe function type.
     *
     * The function must return true when the network is considered online.
     */
    using ProbeFn = std::function<bool()>;

    /**
     * @struct Config
     * @brief Network probe configuration.
     */
    struct Config
    {
      /**
       * @brief Minimum delay between two real probe executions.
       */
      std::int64_t min_interval_ms{1000};

      /**
       * @brief Time-to-live for a cached online result.
       */
      std::int64_t online_ttl_ms{2000};

      /**
       * @brief Time-to-live for a cached offline result.
       */
      std::int64_t offline_ttl_ms{500};

      /**
       * @brief Initial state used before the first probe.
       */
      bool initial_online{false};

      /**
       * @brief State used when no probe function is configured.
       */
      bool fallback_online{false};

      /**
       * @brief Return true if the configuration is usable.
       */
      [[nodiscard]] bool valid() const noexcept
      {
        return min_interval_ms >= 0 &&
               online_ttl_ms >= 0 &&
               offline_ttl_ms >= 0;
      }
    };

    /**
     * @brief Construct a NetworkProbe with default configuration.
     */
    NetworkProbe() = default;

    /**
     * @brief Construct a NetworkProbe from a probe function.
     *
     * @param fn Probe function invoked to test connectivity.
     */
    explicit NetworkProbe(ProbeFn fn);

    /**
     * @brief Construct a NetworkProbe from configuration and probe function.
     *
     * @param cfg Probe configuration.
     * @param fn Probe function invoked to test connectivity.
     */
    NetworkProbe(Config cfg, ProbeFn fn);

    NetworkProbe(const NetworkProbe &) = default;
    NetworkProbe &operator=(const NetworkProbe &) = default;

    NetworkProbe(NetworkProbe &&) noexcept = default;
    NetworkProbe &operator=(NetworkProbe &&) noexcept = default;

    /**
     * @brief Destroy the network probe.
     */
    ~NetworkProbe() = default;

    /**
     * @brief Return true if the current cached state is still valid.
     *
     * @param now_ms Current monotonic time in milliseconds.
     */
    [[nodiscard]] bool cache_valid(std::int64_t now_ms) const noexcept;

    /**
     * @brief Return the current online state.
     *
     * If the cached state is expired and probing is allowed, this method
     * refreshes the state. Otherwise, it returns the cached state.
     *
     * @param now_ms Current monotonic time in milliseconds.
     */
    [[nodiscard]] bool is_online(std::int64_t now_ms);

    /**
     * @brief Force a probe refresh when rate limiting allows it.
     *
     * If the minimum interval has not elapsed, this method returns the cached
     * state without executing the probe function.
     *
     * @param now_ms Current monotonic time in milliseconds.
     */
    bool refresh(std::int64_t now_ms);

    /**
     * @brief Force a probe execution and bypass rate limiting.
     *
     * @param now_ms Current monotonic time in milliseconds.
     */
    bool force_refresh(std::int64_t now_ms);

    /**
     * @brief Return the last known online state.
     */
    [[nodiscard]] bool last_known_online() const noexcept
    {
      return last_online_;
    }

    /**
     * @brief Return the timestamp of the last probe attempt.
     */
    [[nodiscard]] std::int64_t last_probe_at_ms() const noexcept
    {
      return last_probe_at_ms_;
    }

    /**
     * @brief Return the timestamp of the last state update.
     */
    [[nodiscard]] std::int64_t last_update_ms() const noexcept
    {
      return last_update_ms_;
    }

    /**
     * @brief Check whether the network is currently considered online.
     *
     * Compatibility API.
     */
    [[nodiscard]] bool isOnline(std::int64_t now_ms)
    {
      return is_online(now_ms);
    }

    /**
     * @brief Return the last known online state.
     *
     * Compatibility API.
     */
    [[nodiscard]] bool lastKnownOnline() const noexcept
    {
      return last_known_online();
    }

    /**
     * @brief Timestamp of the last probe attempt.
     *
     * Compatibility API.
     */
    [[nodiscard]] std::int64_t lastProbeAtMs() const noexcept
    {
      return last_probe_at_ms();
    }

    /**
     * @brief Return the current probe configuration.
     */
    [[nodiscard]] const Config &config() const noexcept
    {
      return cfg_;
    }

    /**
     * @brief Replace the probe function.
     */
    void set_probe(ProbeFn fn);

    /**
     * @brief Replace the probe configuration.
     */
    void set_config(Config cfg);

  private:
    /**
     * @brief Return true if a real probe can run at the given time.
     */
    [[nodiscard]] bool can_probe(std::int64_t now_ms) const noexcept;

    /**
     * @brief Execute the configured probe function and update cached state.
     */
    bool run_probe(std::int64_t now_ms);

    [[nodiscard]] bool canProbe(std::int64_t now_ms) const noexcept
    {
      return can_probe(now_ms);
    }

  private:
    Config cfg_{};
    ProbeFn probe_{};

    bool last_online_{false};
    std::int64_t last_probe_at_ms_{0};
    std::int64_t last_update_ms_{0};
    bool initialized_{false};
  };

} // namespace vix::net

#endif // VIX_NET_NETWORKPROBE_HPP
