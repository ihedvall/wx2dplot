/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <set>

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/slider.h>

#include "plot/project.h"

namespace plot {

class MasterDisplay : public wxPanel {
public:
  MasterDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);
  void SetProject(Project* project);

  void Redraw();
protected:
  void OnProjectChanged(wxCommandEvent& event);
  void OnScrollChange(wxScrollEvent& event);
  void OnMasterChanged(wxCommandEvent& event);
private:
  Project* project_ = nullptr;
  wxStaticText* min_value_ = nullptr;
  wxStaticText* max_value_ = nullptr;
  wxStaticText* master_absolute_ = nullptr;
  wxStaticText* master_relative_ = nullptr;
  wxSlider* master_ctrl_ = nullptr;

  wxStaticText* min_label_ = nullptr;
  wxStaticText* max_label_ = nullptr;

  std::vector<double> master_list_;
  std::string master_unit_;

  void CalculateMinMax();
  void SetMinMaxValue() const;
  void SetMasterValue() const;

  wxDECLARE_EVENT_TABLE();
};

} // namespace plot

