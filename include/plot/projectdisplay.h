/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "wx/panel.h"
#include "wx/textctrl.h"
#include "wx/checkbox.h"
#include "wx/timer.h"

#include "plot/project.h"

namespace plot {

class ProjectDisplay : public wxPanel {
public:
  ProjectDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);
  ~ProjectDisplay() override;

  void SetProject(Project* project);
  void Redraw();
protected:
  void OnApply(wxCommandEvent& event);
  void OnTimer(wxTimerEvent& event);
  void OnXZero(wxCommandEvent& event);
  void OnYZero(wxCommandEvent& event);
private:
  Project* project_ = nullptr;
  wxTextCtrl* name_ctrl_ = nullptr;
  wxTextCtrl* desc_ctrl_ = nullptr;

  wxCheckBox* x_zero_ctrl_ = nullptr;
  wxCheckBox* y_zero_ctrl_ = nullptr;
  wxButton* apply_button_ = nullptr;
  wxTimer timer_;
  wxDECLARE_EVENT_TABLE();
};

}  // namespace plot


