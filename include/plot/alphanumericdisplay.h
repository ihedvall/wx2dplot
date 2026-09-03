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
  AlphaNumericDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);

  void SetProject(Project* project);
  [[nodiscard]] Project* GetProject() const { return project_; }

  void Redraw();
protected:
  [[nodiscard]] wxString OnGetItemText(long item, long column) const override;
private:
  Project* project_ = nullptr;
  wxDECLARE_EVENT_TABLE();
};

} // namespace plot

