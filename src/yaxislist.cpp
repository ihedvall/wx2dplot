/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/yaxislist.h"
#include "plot/project.h"

namespace plot {
YAxisList::YAxisList(const Project& project) {
  Init(project);
}

const YAxis* YAxisList::GetAxis(size_t index) const {
  return index < y_axis_list_.size() ? &y_axis_list_[index] : nullptr;
}

const Signal* YAxisList::GetSignal(size_t index) const {
  return index < signal_list_.size() ? signal_list_[index] : nullptr;
}

void YAxisList::Init(const Project& project) {
  const auto& signals = project.GetSignals();

  // Calculate the selected signals
  signal_list_.clear();
  for (const auto& signal : signals) {
    if (!signal.IsSelected()) {
      continue;
    }
    auto& values = signal.SignalValues();
    if (values.Size() < 2) { // Must have 2 values to plot
      continue;
    }
    signal_list_.emplace_back(&signal);
  }

  // Time to find the Y-Axis
  y_axis_list_.clear();
  for (const auto* signal : signal_list_) {
    if (signal == nullptr) {
      continue;
    }
    YAxis* existing = FindYAxis(*signal);
    if (existing == nullptr) {
      YAxis y_axis(*signal);
      y_axis_list_.emplace_back(std::move(y_axis));
    } else {
      existing->AddSignal(*signal);
    }
  }
}

YAxis* YAxisList::FindYAxis(const Signal& signal) const {
  YAxis* existing = nullptr;
  const std::string& unit = signal.GetUnit();
  for (auto& y_axis : y_axis_list_) {
    if (y_axis.Unit() == unit) {
      existing = const_cast<YAxis*>(&y_axis);
      break;;
    }
  }
  return existing;
}

} // plot