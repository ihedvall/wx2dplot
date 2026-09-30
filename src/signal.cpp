/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/signal.h"

#include <utility>

namespace plot {

double Signal::GetMin() const {
  if (min_ < max_) {
    return min_;
  }
  const SignalValue min = value_list_.Min();
  return min.IsValid() ? min.GetValue() : 0.0;
}

double Signal::GetMax() const {
  if (min_ < max_) {
    return max_;
  }
  const SignalValue max = value_list_.Max();
  return max.IsValid() ? max.GetValue() : 0.0;
}

std::string Signal::GetEnum(int value) const {
  if (const auto& itr = enum_list_.find(value);
      itr != enum_list_.cend()) {
    return itr->second;
  }
  return {};
}

bool Signal::IsLatestUpdated() const {
  return value_list_.IsLastUpdated();
}

void Signal::SetLatestValue(const SignalValue& value) {
  value_list_.SetLastValue(value);
}

SignalValue Signal::GetLatestValue() const {
  return value_list_.GetLastValue();
}
SignalValueList& Signal::SignalValues() {
  return value_list_;
}
const SignalValueList& Signal::SignalValues() const {
  return value_list_;
}



} // plot