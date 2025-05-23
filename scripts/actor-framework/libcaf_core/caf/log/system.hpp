// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/main/LICENSE.

#pragma once

#include "caf/detail/build_config.hpp"
#include "caf/log/level.hpp"
#include "caf/logger.hpp"

#include <string_view>

namespace caf::log::system {

/// The name of this component in log events.
constexpr std::string_view component = "caf.system";

#if CAF_LOG_LEVEL >= CAF_LOG_LEVEL_TRACE

/// Logs a message with `debug` severity.
/// @param fmt_str The format string (with source location) for the message.
/// @param args Arguments for the format string.
template <class... Ts>
[[nodiscard]] auto trace(format_string_with_location fmt_str, Ts&&... args) {
  return logger::trace(component, fmt_str, std::forward<Ts>(args)...);
}

#else

// A simple dummy type with a destructor to suppress 'unused variable' warnings
// when using `trace` with trace logging disabled.
struct trace_dummy {
  ~trace_dummy() {
  }
};

template <class... Ts>
[[nodiscard]] auto trace(Ts&&...) {
  return trace_dummy{};
}

#endif

/// Logs a message with `debug` severity.
/// @param fmt_str The format string (with source location) for the message.
/// @param args Arguments for the format string.
template <class... Ts>
void debug(format_string_with_location fmt_str, Ts&&... args) {
  logger::log(level::debug, component, fmt_str,
              std::forward<Ts>(args)...);
}

/// Starts a new log event with `debug` severity.
inline auto debug() {
  return logger::log(level::debug, component);
}

/// Logs a message with `info` severity.
/// @param fmt_str The format string (with source location) for the message.
/// @param args Arguments for the format string.
template <class... Ts>
void info(format_string_with_location fmt_str, Ts&&... args) {
  logger::log(level::info, component, fmt_str,
              std::forward<Ts>(args)...);
}

/// Starts a new log event with `info` severity.
inline auto info() {
  return logger::log(level::info, component);
}

/// Logs a message with `warning` severity.
/// @param fmt_str The format string (with source location) for the message.
/// @param args Arguments for the format string.
template <class... Ts>
void warning(format_string_with_location fmt_str, Ts&&... args) {
  logger::log(level::warning, component, fmt_str,
              std::forward<Ts>(args)...);
}

/// Starts a new log event with `warning` severity.
inline auto warning() {
  return logger::log(level::warning, component);
}

/// Logs a message with `error` severity.
/// @param fmt_str The format string (with source location) for the message.
/// @param args Arguments for the format string.
template <class... Ts>
void error(format_string_with_location fmt_str, Ts&&... args) {
  logger::log(level::error, component, fmt_str,
              std::forward<Ts>(args)...);
}

/// Starts a new log event with `error` severity.
inline auto error() {
  return logger::log(level::error, component);
}

} // namespace caf::log::system
