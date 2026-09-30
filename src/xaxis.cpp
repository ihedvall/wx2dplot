/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/xaxis.h"

#include "plot/project.h"

namespace plot {

XAxis::XAxis(const Project& project) {
  Init(project);
}

void XAxis::Init(const Project& project) {
  const auto& signals = project.GetSignals();
  bool first = true;
  std::string x_unit;

  for (const auto& signal : signals) {
    if (!signal.IsSelected()) {
      continue;
    }
    // It is possible to check if it is enabled.
    // If the signal is disabled then it is not included in the plot
    // but it should be included to the x-axis range.

    const auto& values = signal.SignalValues();
    if (values.Size() < 2) {
      continue;
    }
    const SignalValue& min = values.MasterMin();
    const SignalValue& max = values.MasterMax();
    if (!min.IsValid() || !max.IsValid()) {
      continue;
    }
    // The first signal will be used as name and description.
    // If the master has a mixture of units, well bad luck.
    if (first) {
      x_unit = signal.GetMasterUnit();
      std::ostringstream xname;
      xname << signal.GetMasterName();
      if (!x_unit.empty() ) {
        xname << " [" << x_unit << "]";
      }
      name_ = xname.str();
      description_ = signal.GetDescription();
      min_ = min.GetMasterValue();
      max_ = max.GetMasterValue();
      first = false;
    } else {
      const std::string& unit = signal.GetMasterUnit();
      if (x_unit.empty()) {
        x_unit = unit;
      } else if (!unit.empty() && x_unit != signal.GetMasterUnit()) {
        // In this case, the unit should be converted to the x-axis unit.
        // This require some serious ODS unit software which might be included
        // later on.
        x_axis_unit_mismatch_ = true;
        PLOT_ERROR() << "Unit mismatch on X-axis. "
          << "Signal: " << signal.GetMasterName() << " [" << unit << "]"
          << " X-Axis Unit: " << x_unit;
        continue; // Propably better to ignore these X values
      }
      min_ = std::min(min_, min.GetMasterValue());
      max_ = std::max(max_, max.GetMasterValue());
    }
  }

  if (project.GetIncludeZeroX()) {
    if (min_ > 0.0) {
      min_ = 0.0;
    }
    if (max_ < 0.0) {
      max_ = 0.0;
    }
  }

}

} // plot