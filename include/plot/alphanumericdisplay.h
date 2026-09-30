/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <wx/listctrl.h>

#include "plot/project.h"
namespace plot {

class AlphaNumericDisplay : public wxListView {
public:
  explicit AlphaNumericDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);

  void SetProject(Project* project);
  [[nodiscard]] Project* GetProject() const { return project_; }

  void Redraw();
protected:
  [[nodiscard]] bool OnGetItemIsChecked(long item) const override;
  [[nodiscard]] wxString OnGetItemText(long item, long column) const override;
  void OnItemChecked(wxListEvent& event);
  void OnItemUnchecked(wxListEvent& event);
  void OnProjectChanged(wxCommandEvent& event);
  void OnMasterChanged(wxCommandEvent& event);
private:
  Project* project_ = nullptr;
  wxDECLARE_EVENT_TABLE();
};

} // namespace plot

