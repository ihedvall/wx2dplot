/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/splitter.h>

#include "plot/alphanumericdisplay.h"
#include "plot/lineplotdisplay.h"
#include "plot/masterdisplay.h"
#include "plot/legenddisplay.h"
#include "plot/projectdisplay.h"
#include "plot/project.h"

namespace plot {

class LinePlotWindow : public wxPanel {
public:
  LinePlotWindow() = default;
  explicit LinePlotWindow(wxWindow *parent, wxWindowID id = wxID_ANY);;

  void SetProject(Project* project);
  void Redraw() const;

protected:
  void OnProjectChanged(wxCommandEvent& event);
private:
  AlphaNumericDisplay* alphanumeric_display_ = nullptr;
  LinePlotDisplay* lineplot_display_ = nullptr;
  LegendDisplay* legend_display_ = nullptr;
  MasterDisplay* master_display_ = nullptr;
  ProjectDisplay* project_display_ = nullptr;
  wxSplitterWindow* main_splitter_ = nullptr;

  Project* project_ = nullptr;

  wxDECLARE_DYNAMIC_CLASS(LinePlotFrame);
  wxDECLARE_EVENT_TABLE();
};

}
