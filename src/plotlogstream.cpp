/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/plotlogstream.h"

#include <iostream>
#include <array>
#include <string_view>
#include <vector>
#include <chrono>
#include <format>
#include <locale>

using namespace std::chrono;
using namespace std::chrono_literals;

namespace {

std::vector<plot::PlotLogFunction> kLogFunctionList;

constexpr std::array<std::string_view, 9> kSeverityList = {
  "Trace", "Debug", "Info",
  "Notice", "Warning", "Error",
  "Critical","Alert", "Emergency" };

plot::PlotLogSeverity kLogLevel = plot::PlotLogSeverity::kTrace;
bool kShowLocation = true;
}  // end namespace

namespace plot {

PlotLogStream::PlotLogStream(const std::source_location &location, PlotLogSeverity severity)
    : location_(location), severity_(severity) {
  // Change the below line if you not using english comments.
  // This line ensure standard C-language outputs of integers and floating
  // points outputs.
  imbue(std::locale::classic());
}

PlotLogStream::~PlotLogStream() {
  PlotLogStream::LogString(location_, severity_, str());
}

void PlotLogStream::LogString(const std::source_location &location,
    PlotLogSeverity severity, const std::string &text) {
  for (const auto &func : kLogFunctionList) {
    if (func) {
      func(location, severity, text);
    }
  }
}

void PlotLogStream::SetLogFunction(const PlotLogFunction &func) {
  kLogFunctionList.emplace_back(func);
}

void PlotLogStream::ResetLogFunction() {
  kLogFunctionList.clear();
}

void PlotLogStream::LogToConsole(const std::source_location& location,
           PlotLogSeverity severity, const std::string& text) {
  if (severity < kLogLevel) {
    return;
  }
  try {
    const auto now = system_clock::now();
    const auto time_zone = current_zone();
    const auto local_time = time_zone->to_local(now);
    const hh_mm_ss time_of_day(local_time - floor<days>(local_time));
    std::ostringstream time_string;
    time_string.imbue(std::locale(""));
    time_string << std::format("{:%T}", time_of_day); ;

    std::cout << time_string.str() << " ";
    if (const auto index = static_cast<size_t>(severity);
        index < kSeverityList.size()) {
      std::cout << "[" << kSeverityList[index] << "] ";
    }

    std::cout << text;
    if (kShowLocation) {
      std::cout << " (" << location.file_name() << ":"
                << location.function_name()
                << " C:" << location.column()
                << " L:" << location.line() << ")";
    }
    std::cout << std::endl;
  } catch (const std::exception& err) {
    std::cout << "Formating error. Error: " << err.what() << std::endl;
  }
}

void PlotLogStream::SetLogLevel(PlotLogSeverity severity) {
  kLogLevel = severity;
}

void PlotLogStream::ShowLocation(bool show_location) {
  kShowLocation = show_location;
}

}  // namespace plot