/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/vector.h>
#include <wx/bmpbndl.h>

#include "plot/project.h"

namespace plot {

class LegendDisplay : public wxPanel {
 public:
  LegendDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);
  void SetProject(Project* project);
  void Redraw();
 protected:
  void OnProjectChanged(wxCommandEvent& event);
  void OnMasterChanged(wxCommandEvent& event);
  void RedrawY() const;
  void RedrawX() const;
 private:
  Project* project_ = nullptr;
  wxVector<wxBitmapBundle> color_list_;
  wxListView* y_list_view_ = nullptr;
  wxListView* x_list_view_ = nullptr;
  void CreateColorList();
  wxDECLARE_EVENT_TABLE();
};

}  // namespace plot


