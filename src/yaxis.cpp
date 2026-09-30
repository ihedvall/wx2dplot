//
// Created by ihedv on 2026-09-21.
//

#include "plot/yaxis.h"

#include "plot/signal.h"

namespace plot {
YAxis::YAxis(const Signal& signal) {
  // Check if the signal or value list defines the range
  const auto& values = signal.SignalValues();

  min_ = signal.GetMin();
  max_ = signal.GetMax();
  unit_ = signal.GetUnit();
  description_ = signal.GetDescription();
  signal_list_.insert(&signal);
}

std::string YAxis::Name() const {
  std::ostringstream name;
  bool first = true;
  for (const auto& signal : signal_list_) {
    if (first) {
      name << signal->GetName();
      first = false;
    } else {
      name << ", " << signal->GetName();
    }
  }
  if (!unit_.empty()) {
    name << " [" << unit_ << "]";
  }
  return name.str();
}

void YAxis::AddSignal(const Signal& signal) {
  // Check if the signal or value list defines the range
  const auto& values = signal.SignalValues();
  double min = signal.GetMin();
  double max = signal.GetMax();
  min_ = std::min(min_, min);
  max_ = std::max(max_, max);
  signal_list_.insert(&signal);
}

} // plot