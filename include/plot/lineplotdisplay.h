/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mathplot.h>

#include "plot/project.h"
#include "plot/xaxis.h"
#include "plot/yaxislist.h"
#include "mpinfoxtracker.h"

namespace plot {

class LinePlotDisplay : public mpWindow {
public:
  LinePlotDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);
  void SetProject(Project* project);
  void Redraw();
protected:
  void OnProjectChanged(wxCommandEvent& event);
  void OnMasterChanged(wxCommandEvent& event);
private:
  Project* project_ = nullptr;
  mpInfoXTracker* tracker_ = nullptr;
  XAxis x_axis_;
  YAxisList y_axis_list_;
  void OnXEvent(double x_value);
};

} // namespace plot


