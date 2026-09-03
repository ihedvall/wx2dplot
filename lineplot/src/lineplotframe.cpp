/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "lineplotframe.h"

#include <filesystem>

#include "lineplotapp.h"
#include "lineplotdocument.h"
#include "mainframe.h"
#include "windowid.h"

namespace plot {

wxIMPLEMENT_DYNAMIC_CLASS(LinePlotFrame,wxPanel)

wxBEGIN_EVENT_TABLE(LinePlotFrame, wxPanel)
wxEND_EVENT_TABLE()

LinePlotFrame::LinePlotFrame(wxWindow *parent)
    : wxPanel( parent ) {

  splitter_ = new wxSplitterWindow(this);
  splitter_->SetMinimumPaneSize(100);
  lineplot_display_ = new LinePlotDisplay(splitter_);
  alphanumeric_display_ = new AlphaNumericDisplay(splitter_);

  splitter_->SplitHorizontally(lineplot_display_,
    alphanumeric_display_,
    300);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(splitter_, 1 , wxALL | wxGROW,0);
  SetSizer(main_sizer);
  wxPanel::Layout();

  CallAfter([this]() -> void  {
    if (splitter_ != nullptr && splitter_->IsSplit()) {
      splitter_->SetSashPosition(300);
      splitter_->Layout();
    }
  });

}

void LinePlotFrame::Redraw() const {

}

LinePlotDocument *LinePlotFrame::GetDocument() {
  return MainFrame::GetDocument();
}

void LinePlotFrame::SetView(LinePlotView *view) {
  view_ = view;
}

} // end namespace plot



