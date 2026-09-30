/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/sinusproject.h"

#include <array>
#include <numbers>
#include <cmath>
#include <chrono>

using namespace std::chrono;
namespace {

double SinusValue(size_t index) {
  return 10 * std::sin((static_cast<double>(index) / 360.0) *
                         (2.0 * std::numbers::pi));
}
double CosinusValue(size_t index) {
  return 10 * std::cos((static_cast<double>(index) / 360.0) *
                         (2.0 * std::numbers::pi));
}

}
namespace plot {

SinusProject::SinusProject()
: Project() {
  SetName("Sinus Demo Project");
  SetDescription("This is a demo project for a sinus wave");
  const auto now =
    time_point_cast<nanoseconds>(system_clock::now());
  SetUtcBaseTime(static_cast<uint64_t>(now.time_since_epoch().count()));
  {
    Signal sinus;
    sinus.SetSelected(true);
    sinus.SetName("Sinus");
    sinus.SetDescription("This is a sinus wave");
    sinus.SetUnit("V");
    sinus.SetMasterName("Time");
    sinus.SetMasterUnit("s");
    sinus.SetMin(-10.0);
    sinus.SetMax(10.0);
    sinus.SetPrecision(2);
    sinus.SetMasterPrecision(3);

    std::array<SignalValue, 360> value_list {};
    double sample_time = 0.0;
    for (size_t index = 0; index < 360; ++index) {
      const double value = SinusValue(index);
      auto& signal_value = value_list[index];
      signal_value.SetValue(value, sample_time, true);
      sample_time += 0.01;
    }
    sinus.SignalValues().AttachValues(value_list);
    AddSignal(sinus);
  }
  {
    Signal cosinus;
    cosinus.SetSelected(true);
    cosinus.SetName("Cosinus");
    cosinus.SetDescription("This is a cosinus wave");
    cosinus.SetUnit("V");
    cosinus.SetMasterName("Time");
    cosinus.SetMasterUnit("s");
    cosinus.SetMin(-10.0);
    cosinus.SetMax(10.0);
    cosinus.SetPrecision(2);
    cosinus.SetMasterPrecision(3);

    std::array<SignalValue, 360> value_list {};
    double sample_time = 0.0;
    for (size_t index = 0; index < 360; ++index) {
      const double value = CosinusValue(index);
      auto& signal_value = value_list[index];
      signal_value.SetValue(value, sample_time, true);
      sample_time += 0.01;
    }
    cosinus.SignalValues().AttachValues(value_list);
    AddSignal(cosinus);
  }
  SortSignals();
}

} // plot