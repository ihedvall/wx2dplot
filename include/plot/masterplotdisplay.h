/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mathplot.h>

#include "plot/project.h"
#include "plot/xaxis.h"

#include "mpinfoxtracker.h"

namespace plot {

class MasterPlotDisplay : public mpWindow {
public:
  MasterPlotDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);
  void SetProject(Project* project);
  void Redraw();
protected:
  void OnMouseMove(wxMouseEvent &event) override;
  void OnProjectChanged(wxCommandEvent& event);
  void OnMasterChanged(wxCommandEvent& event);
private:
  Project* project_ = nullptr;
  mpInfoXTracker* tracker_ = nullptr;
  XAxis x_axis_;
  void OnXEvent(double x_value) const;
  void UnbindEvents();
};

}

