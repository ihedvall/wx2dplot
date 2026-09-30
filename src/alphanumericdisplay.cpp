/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/alphanumericdisplay.h"
#include "plot/project.h"

#include <set>
namespace plot {

wxBEGIN_EVENT_TABLE(AlphaNumericDisplay, wxListView)
wxEND_EVENT_TABLE()

AlphaNumericDisplay::AlphaNumericDisplay(wxWindow *parent, wxWindowID id)
  : wxListView(parent, id, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL) {
  wxListCtrl::EnableCheckBoxes(true);
  wxListCtrl::SetMinSize({100, -1});
  InsertColumn(0, "Use", wxLIST_FORMAT_LEFT, 50);
  InsertColumn(1, "Name",wxLIST_FORMAT_LEFT, 200);
  InsertColumn(2, "Value",wxLIST_FORMAT_RIGHT, 100);
  InsertColumn(3, "Unit",wxLIST_FORMAT_LEFT, 75);
  InsertColumn(4, "Description",wxLIST_FORMAT_LEFT, 300);

  Bind(wxEVT_LIST_ITEM_CHECKED, &AlphaNumericDisplay::OnItemChecked,
    this);
  Bind(wxEVT_LIST_ITEM_UNCHECKED, &AlphaNumericDisplay::OnItemUnchecked,
    this);
}

void AlphaNumericDisplay::SetProject(Project *project) {
  const auto* old_project = project_;
  project_ = project;
  if (project_ != nullptr && old_project != project_) {
    project_->Bind(PROJECT_CHANGED, &AlphaNumericDisplay::OnProjectChanged,
      this);
    project_->Bind(MASTER_CHANGED, &AlphaNumericDisplay::OnMasterChanged,
      this);
    project_->Bind(PLOT_MASTER_CHANGED, &AlphaNumericDisplay::OnMasterChanged,
      this);
  }
  if (project_ == nullptr) {
    Redraw();
  }
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
bool AlphaNumericDisplay::OnGetItemIsChecked(long item) const {
  if (project_ == nullptr || item < 0) {
    return false;
  }
  const Signal* signal = project_->GetSignal(item);
  if (signal == nullptr) {
    return false;
  }
  return signal->IsSelected();
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
    const int precision = signal->GetPrecision();
    const double x_value = project_->GetMasterReference();
    const SignalValue value = signal->SignalValues().GetValueByMaster(x_value);
    if (!value.IsValid()) {
      text = "*";
      break;
    }

    if (const auto& enum_list = signal->GetEnumList();
        !enum_list.empty()) {
      const std::string enum_value = signal->GetEnum(
        static_cast<int>(value.GetValue()));
      text = wxString::FromUTF8(enum_value);
    } else {
      text = wxString::FromDouble(value.GetValue(), precision);
    }
    break;
  }
  case 3:  //
    text = wxString::FromUTF8(signal->GetUnit());
    break;

  case 4:  // Description
    text = wxString::FromUTF8(signal->GetDescription());
    break;

  default:
    break;
  }
  return text;
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void AlphaNumericDisplay::OnItemChecked(wxListEvent& event) {
  if (project_ == nullptr) {
    return;
  }
  const long item = event.GetIndex();
  if (Signal* signal = project_->GetSignal(item); signal != nullptr) {
    signal->SetSelected(true);
  }
  wxCommandEvent changed(PROJECT_CHANGED);
  project_->ProcessEventLocally(changed);

}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void AlphaNumericDisplay::OnItemUnchecked(wxListEvent& event) {
  if (project_ == nullptr) {
    return;
  }
  const long item = event.GetIndex();
  if (Signal* signal = project_->GetSignal(item); signal != nullptr) {
    signal->SetSelected(false);
  }
  wxCommandEvent event1(PROJECT_CHANGED);
  project_->ProcessEventLocally(event1);
}

void AlphaNumericDisplay::OnProjectChanged(wxCommandEvent& event) {
  Redraw();
  event.Skip();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AlphaNumericDisplay::OnMasterChanged(wxCommandEvent& event) {
  event.Skip();
  if (project_ == nullptr) {
    return;
  }
  // Update all value deoendent on the X-value.
  // Set default value to false.
  RefreshItems(0,
    static_cast<long>(GetItemCount() - 1));

}

} // plot