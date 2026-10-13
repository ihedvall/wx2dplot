/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/legenddisplay.h"

#include <set>
#include <string>
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <wx/sizer.h>

#include <mpcolourscheme.h>

namespace {

wxBitmap CreateSolidBitmap16x16(const wxColour& color) {
  wxBitmap bitmap(16, 16);

  wxMemoryDC dc(bitmap);
  dc.SetBackground(*wxTRANSPARENT_BRUSH);
  dc.Clear();
  // Draw a centered 12x12 box.
  // Offset is 2 pixels on each side: (16 - 12) / 2 = 2.
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(wxBrush(color));
  dc.DrawRectangle(2, 2, 12, 12);
  dc.SelectObject(wxNullBitmap);

  return bitmap;
}

}

namespace plot {
wxBEGIN_EVENT_TABLE(LegendDisplay, wxPanel)
wxEND_EVENT_TABLE()

LegendDisplay::LegendDisplay(wxWindow *parent, wxWindowID id)
  : wxPanel(parent, id),
   y_list_view_(new wxListView(this, wxID_ANY,wxDefaultPosition,
     wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL)),
   x_list_view_(new wxListView(this, wxID_ANY,wxDefaultPosition,
     wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL))
{
  CreateColorList();
  wxPanel::SetMinSize({350, 250});

  y_list_view_->SetMinSize({-1, 200});
  y_list_view_->SetSmallImages(color_list_);
  y_list_view_->InsertColumn(0, "Name",wxLIST_FORMAT_LEFT, 200);
  y_list_view_->InsertColumn(1, "Value",wxLIST_FORMAT_RIGHT, 75);
  y_list_view_->InsertColumn(2, "Unit",wxLIST_FORMAT_LEFT, 75);

  x_list_view_->SetMinSize({-1, 50});
  x_list_view_->SetSmallImages(color_list_);
  x_list_view_->InsertColumn(0, "Name",wxLIST_FORMAT_LEFT, 200);
  x_list_view_->InsertColumn(1, "Value",wxLIST_FORMAT_RIGHT, 75);
  x_list_view_->InsertColumn(2, "Unit",wxLIST_FORMAT_LEFT, 75);

  auto* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(y_list_view_, 1, wxGROW);
  sizer->Add(x_list_view_, 0, wxEXPAND);
  SetSizerAndFit(sizer);
}

void LegendDisplay::SetProject(Project* project) {
  const auto* old_project = project_;
  project_ = project;
  if (project_ != nullptr && old_project != project_) {
    project_->Bind(PROJECT_CHANGED, &LegendDisplay::OnProjectChanged,
      this);
    project_->Bind(MASTER_CHANGED, &LegendDisplay::OnMasterChanged,
      this);
    project_->Bind(PLOT_MASTER_CHANGED, &LegendDisplay::OnMasterChanged,
      this);
  }
  if (project_ == nullptr) {
    Redraw();
  }
}

void LegendDisplay::Redraw() {
  RedrawY();
  RedrawX();
  Refresh();
}

void LegendDisplay::RedrawY() const {
  const auto& scheme = mpColourScheme::Instance();
  if (y_list_view_ == nullptr) {
    return;
  }

  // Save the selected items so they still is selected after redraw
  std::set<std::string> selected_list;
  for (long selected = y_list_view_->GetFirstSelected(); selected >= 0;
            selected = y_list_view_->GetNextSelected(selected)) {
    wxString sel = y_list_view_->GetItemText(selected, 1);
    if (!sel.empty()) {
      selected_list.emplace(sel.utf8_string());
    }
            }
  y_list_view_->DeleteAllItems();

  if (project_ == nullptr) {
    return;
  }

  const double x_value = project_->GetMasterReference();
  // const Signal* master_signal = nullptr;

  const auto& signal_list = project_->GetSignals();
  long line = 0;
  for (const Signal& signal : signal_list) {
    if (!signal.IsActive()) {
      continue;
    }

    const int color_index = line % static_cast<int>(scheme.GetNofIndexColours());
    const int image_index = color_index + 1;
    const int precision = signal.GetPrecision();

    const long item = y_list_view_->InsertItem(line,
      wxString::FromUTF8(signal.GetName()), image_index);
    const SignalValue value = signal.SignalValues().GetValueByMaster(x_value);
    if (!value.IsValid()) {
      y_list_view_->SetItem(item, 1, "*");
    } else {
      y_list_view_->SetItem(item, 1,
        wxString::FromDouble(value.GetValue(), precision));
    }
    y_list_view_->SetItem(item, 2,
      wxString::FromUTF8(signal.GetUnit()));
    ++line;
  }

  for ( const std::string& selname : selected_list) {
    if (const long find = project_->GetSignal(selname); find >= 0) {
      y_list_view_->Select(find);
      y_list_view_->EnsureVisible(find);
    }
  }

}

void LegendDisplay::RedrawX() const {
  if (x_list_view_ == nullptr) {
    return;
  }

  // Save the selected items so they still is selected after redraw
  bool select_x = false;
  for (long selected = x_list_view_->GetFirstSelected(); selected >= 0;
            selected = x_list_view_->GetNextSelected(selected)) {
    select_x = true;
  }
  x_list_view_->DeleteAllItems();

  if (project_ == nullptr) {
    return;
  }

  const double x_value = project_->GetMasterReference();
  const Signal* master_signal = nullptr;

  const auto& signal_list = project_->GetSignals();
  for (const Signal& signal : signal_list) {
    if (!signal.IsActive()) {
      continue;
    }
    if (master_signal == nullptr) {
      master_signal = &signal;
    }

  }
  if (master_signal != nullptr) {
    const int precision = master_signal->GetMasterPrecision();
    const long item = x_list_view_->InsertItem(0,
      wxString::FromUTF8(master_signal->GetMasterName()), 0);
    x_list_view_->SetItem(item, 1,
      wxString::FromDouble(x_value, precision));
    x_list_view_->SetItem(item, 2,
      wxString::FromUTF8(master_signal->GetMasterUnit()));
  }

  if (select_x) {
    x_list_view_->Select(0);
    x_list_view_->EnsureVisible(0);
   }
}

void LegendDisplay::OnProjectChanged(wxCommandEvent& event) {
  Redraw();
  event.Skip();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LegendDisplay::OnMasterChanged(wxCommandEvent& event) {
  event.Skip();
  if (project_ == nullptr) {
    return;
  }
  const Signal* master_signal = nullptr;
  const double x_value = project_->GetMasterReference();
  const auto& signal_list = project_->GetSignals();
  int line = 0;
  for (const Signal& signal : signal_list) {
    if (!signal.IsActive()) {
      continue;
    }
    master_signal = &signal;
    const int precision = signal.GetPrecision();
    const auto item = static_cast<long>(line);
    const SignalValue value = signal.SignalValues().GetValueByMaster(x_value);
    if (!value.IsValid()) {
      y_list_view_->SetItem(item, 1, "*");
    } else {
      y_list_view_->SetItem(item, 1, wxString::FromDouble(value.GetValue(), precision));
    }
    ++line;
  }
  if (master_signal != nullptr)
  {
    const int precision = master_signal->GetPrecision();
    x_list_view_->SetItem(0, 1,
      wxString::FromDouble(x_value, precision));
  }
}

void LegendDisplay::CreateColorList() {
  color_list_.clear();
  const auto& scheme = mpColourScheme::Instance();
  wxBitmap axis_bmp = CreateSolidBitmap16x16(scheme.GetAxisColour());
  color_list_.emplace_back(std::move(axis_bmp));

  for (size_t color_index = 0;
       color_index < scheme.GetNofIndexColours();
       ++color_index) {
    wxColor color = scheme.GetIndexColour(color_index);
    wxBitmap bmp = CreateSolidBitmap16x16(color);
    color_list_.emplace_back(std::move(bmp));
  }
}

} // plot