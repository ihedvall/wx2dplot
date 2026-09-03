/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/alphanumericdisplay.h"

#include <set>
namespace plot {

wxBEGIN_EVENT_TABLE(AlphaNumericDisplay, wxListView)

wxEND_EVENT_TABLE()
AlphaNumericDisplay::AlphaNumericDisplay(wxWindow *parent, wxWindowID id)
  : wxListView(parent, id, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL) {
  wxListCtrl::EnableCheckBoxes(true);
  InsertColumn(0, "Plot", wxLIST_FORMAT_LEFT, 50);
  InsertColumn(1, "Name",wxLIST_FORMAT_LEFT, 200);
  InsertColumn(2, "Value",wxLIST_FORMAT_LEFT, 100);
  InsertColumn(3, "Change",wxLIST_FORMAT_LEFT, 50);
  InsertColumn(4, "Unit",wxLIST_FORMAT_LEFT, 75);
  InsertColumn(5, "Color",wxLIST_FORMAT_LEFT, 75);
  InsertColumn(6, "Description",wxLIST_FORMAT_LEFT, 300);
}

void AlphaNumericDisplay::SetProject(Project *project) {
  project_ = project;
  Redraw();
  Update();
}

void AlphaNumericDisplay::Redraw() {
  // Save the selected items so they still is selected after redraw
  std::set<std::string> selected_list;
  for (long selected = GetFirstSelected(); selected >= 0;
            selected = GetNextSelected(selected)) {
    wxString sel = GetItemText(selected, 1);
    if (!sel.empty()) {
      selected_list.emplace(sel.utf8_string());
    }
  }
  DeleteAllItems();

  if (project_ == nullptr) {
    return;
  }

  const auto& signal_list = project_->GetSignals();
  const size_t item_count = signal_list.size();
  SetItemCount(static_cast<long>(item_count));

  for ( const std::string& selname : selected_list) {
    if (const long find = project_->GetSignal(selname); find >= 0) {
      Select(find);
      EnsureVisible(find);
    }
  }

  if (item_count > 0) {
    RefreshItems(0, static_cast<long>(item_count) - 1);
  } else {
    Refresh();
  }

}

wxString AlphaNumericDisplay::OnGetItemText(long item, long column) const {
  wxString text;
  if (project_ == nullptr || item < 0) {
    return text;
  }
  const Signal* signal = project_->GetSignal(item);
  if (signal == nullptr) {
    return text;
  }

  switch (column) {
  case 0:
    break;

  case 1: // Name
    text = wxString::FromUTF8(signal->GetName());
    break;

  case 2: {
    // Value
    double value = 0.0;
    double master = 0.0;
    const bool valid = signal->GetLatestValue(value, master);
    if (!valid) {
      text = "*";
      break;
    }

    if (const auto& enum_list = signal->GetEnumList();
        !enum_list.empty()) {
      const std::string enum_value = signal->GetEnum(static_cast<int>(value));
      text = wxString::FromUTF8(enum_value);
        } else {
          text = wxString::FromDouble(value);
        }
    break;
  }

  case 3: // Change
    break;

  case 4:  //
    text = wxString::FromUTF8(signal->GetUnit());
    break;

  case 5: // Colour
    break;

  case 6:  // Description
    text = wxString::FromUTF8(signal->GetDescription());
    break;

  default:
    break;
  }
  return text;
}

} // plot