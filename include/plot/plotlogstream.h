/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file plotlogstream.h
 * \brief The plot log stream file is intended to isolate the logging so
 * the library can be built without dependency of the util library.
 */
#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <source_location>

namespace plot {

/** \brief Defines the log severity level. */
enum class PlotLogSeverity : uint8_t {
  kTrace = 0,  ///< Trace or listen message
  kDebug,      ///< Debug message
  kInfo,       ///< Informational message
  kNotice,     ///< Notice message. Notify the user.
  kWarning,    ///< Warning message
  kError,      ///< Error message
  kCritical,   ///< Critical message (device error)
  kAlert,      ///< Alert or alarm message
  kEmergency   ///< Fatal error message
};

#define PLOT_TRACE() PlotLogStream(std::source_location::current(),\
                           PlotLogSeverity::kTrace) ///< Trace log message
#define PLOT_DEBUG() PlotLogStream(std::source_location::current(), \
                           PlotLogSeverity::kDebug)  ///< Debug log message
#define PLOT_INFO() PlotLogStream(std::source_location::current(), \
                           PlotLogSeverity::kInfo)  ///< Info log message
#define PLOT_ERROR() PlotLogStream(std::source_location::current(), \
                           PlotLogSeverity::kError)  ///< Error log message

/** \brief Plot log function definition. */
using PlotLogFunction = std::function<void(const std::source_location &location,
  PlotLogSeverity severity, const std::string &text)>;

/** \brief MDF log stream interface.
 *
 *
 */
class PlotLogStream : public std::ostringstream {
 public:
  PlotLogStream(const std::source_location &location, PlotLogSeverity severity);  ///< Constructor
  ~PlotLogStream() override;                                    ///< Destructor

  PlotLogStream() = delete;
  PlotLogStream(const PlotLogStream&) = delete;
  PlotLogStream(PlotLogStream&&) = delete;
  PlotLogStream& operator=(const PlotLogStream&) = delete;
  PlotLogStream& operator=(PlotLogStream&&) = delete;

  /** \brief Sets a log function. */
  static void SetLogFunction(const PlotLogFunction& func);
  /** \brief Sets a log function. */
  static void ResetLogFunction();

  static void LogToConsole( const std::source_location& location,
                            PlotLogSeverity severity,
                            const std::string& text);
  static void SetLogLevel(PlotLogSeverity severity);
  static void ShowLocation(bool show_location);
 protected:
  std::source_location location_;     ///< File and function location.
  PlotLogSeverity severity_;           ///< Log level of the stream

  /** \brief Defines the logging function. */
  virtual void LogString(const std::source_location& location, PlotLogSeverity severity,
                         const std::string& text);

};

}  // namespace mdf

