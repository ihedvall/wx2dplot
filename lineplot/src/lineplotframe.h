/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/wx.h>

#include "lineplotdocument.h"
#include "plot/alphanumericdisplay.h"
#include "plot/lineplotdisplay.h"

namespace plot {
class LinePlotView;

class LinePlotFrame : public wxPanel {
public:
  LinePlotFrame() = default;
  explicit LinePlotFrame(wxWindow *parent);
  void SetView(LinePlotView* view);
  void Redraw() const;

  [[nodiscard]] static LinePlotDocument* GetDocument();

private:
  AlphaNumericDisplay* alphanumeric_display_ = nullptr;
  LinePlotDisplay* lineplot_display_ = nullptr;
  wxSplitterWindow* splitter_ = nullptr;
  LinePlotView* view_ = nullptr;

  wxDECLARE_DYNAMIC_CLASS(LinePlotFrame);
  wxDECLARE_EVENT_TABLE();
};

}
