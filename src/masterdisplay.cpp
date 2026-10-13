/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/masterdisplay.h"

#include <sstream>
#include <chrono>
#include <format>
#include <algorithm>

#include <wx/sizer.h>

using namespace std::chrono;

namespace plot {
wxBEGIN_EVENT_TABLE(MasterDisplay, wxPanel)
wxEND_EVENT_TABLE()

MasterDisplay::MasterDisplay(wxWindow *parent, wxWindowID id)
  : wxPanel(parent, id) {

  min_label_ = new wxStaticText(this, wxID_ANY, "X-Scale Min ",
    wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
  min_value_ = new wxStaticText(this, wxID_ANY, "*",
    wxDefaultPosition, {100,-1}, wxALIGN_LEFT);

  max_label_ = new wxStaticText(this, wxID_ANY, "X-Scale Max",
    wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
  max_value_ = new wxStaticText(this, wxID_ANY, "*",
    wxDefaultPosition, {100,-1}, wxALIGN_RIGHT);

  master_relative_ = new wxStaticText(this, wxID_ANY, "*",
    wxDefaultPosition, {100,-1}, wxALIGN_RIGHT);
  wxFont bold_font = master_relative_->GetFont();
  bold_font.MakeBold();
  master_relative_->SetFont(bold_font);

  master_absolute_ = new wxStaticText(this, wxID_ANY, "*",
    wxDefaultPosition, {300,-1}, wxALIGN_LEFT);
  master_absolute_->SetFont(bold_font);

  master_ctrl_ = new wxSlider(this, wxID_ANY,
    0,0, 100,  wxDefaultPosition, wxDefaultSize,
    wxSL_HORIZONTAL );

  master_ctrl_->Bind(wxEVT_SCROLL_CHANGED,
    &MasterDisplay::OnScrollChange, this);
  master_ctrl_->Bind(wxEVT_SCROLL_THUMBTRACK,
    &MasterDisplay::OnScrollChange, this);

  auto* left_sizer = new wxBoxSizer(wxHORIZONTAL);
  left_sizer->Add(min_label_, 0, wxALL, 5  );
  left_sizer->Add(min_value_,0, wxALL, 5);

  auto* center_sizer = new wxBoxSizer(wxHORIZONTAL);
  center_sizer->Add(master_relative_, 0, wxALL, 5 );
  center_sizer->Add(master_absolute_, 0, wxALL, 5 );

  auto* right_sizer = new wxBoxSizer(wxHORIZONTAL);
  right_sizer->Add(max_value_,0, wxALL, 5);
  right_sizer->Add(max_label_, 0, wxALL, 5 );


  auto* master_sizer = new wxBoxSizer(wxHORIZONTAL);
  master_sizer->Add(left_sizer, 0, 0, 5);
  master_sizer->AddStretchSpacer();
  master_sizer->Add(center_sizer, 0, 0, 5 );
  master_sizer->AddStretchSpacer();
  master_sizer->Add(right_sizer, 0, 0, 5);

  auto* ctrl_sizer = new wxBoxSizer(wxHORIZONTAL);
  ctrl_sizer->Add(master_ctrl_, 1, wxEXPAND, 5 );

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(master_sizer, 0,  wxEXPAND, 5);
  main_sizer->Add(ctrl_sizer, 0, wxEXPAND, 5 );

  SetSizerAndFit(main_sizer);
}

void MasterDisplay::SetProject(Project* project) {
  const auto* old_project = project_;
  project_ = project;
  if (project_ != nullptr && old_project != project_) {
    project_->Bind(PROJECT_CHANGED, &MasterDisplay::OnProjectChanged,
      this);
    project_->Bind(MASTER_CHANGED, &MasterDisplay::OnMasterChanged,
      this);
  }
  if (project_ == nullptr) {
    Redraw();
  }
}

void MasterDisplay::Redraw() {
  CalculateMinMax();
  SetMinMaxValue();
  SetMasterValue();
}

void MasterDisplay::OnProjectChanged(wxCommandEvent& event) {
  if (master_ctrl_ != nullptr) {
    master_ctrl_->SetValue(0);
  }
  Redraw();
  event.Skip();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void MasterDisplay::OnScrollChange(wxScrollEvent& event) {
  if (master_ctrl_ == nullptr) {
    return;
  }
  const int position = std::clamp(event.GetPosition(), 0,
    static_cast<int>(master_list_.size()));
  if (position >= static_cast<int>(master_list_.size()) ) {
    return;
  }
  const double master_value = master_list_[position];
  if (project_ != nullptr) {
    project_->SetMasterReference(master_value);
    wxCommandEvent changed(MASTER_CHANGED);
    project_->ProcessEventLocally(changed);
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void MasterDisplay::OnMasterChanged(wxCommandEvent& event) { // NOLINT(*-convert-member-functions-to-static)
  SetMasterValue();
  event.Skip();
}

void MasterDisplay::CalculateMinMax() {
  master_list_.clear();
  master_list_.shrink_to_fit();
  master_unit_.clear();
  if (project_ == nullptr) {
    return;
  }
  std::set<double> sorted_list;
  for (const Signal& signal : project_->GetSignals()) {
    if (!signal.IsActive()) {
      continue;
    }
    master_unit_ = signal.GetMasterUnit();
    signal.SignalValues().GetMasterList(sorted_list);
  }
  try {
    master_list_.resize(sorted_list.size());
    master_list_.assign(sorted_list.cbegin(), sorted_list.cend());
  } catch (const std::exception& ) {
    master_list_.clear();
  }
}

void MasterDisplay::SetMinMaxValue() const {
  if (min_value_ == nullptr ||
      max_value_ == nullptr ||
      master_ctrl_ == nullptr) {
    return;
  }

  if (master_list_.empty()) {
    min_value_->SetLabel("*");
    max_value_->SetLabel("*");
  } else if (master_list_.size() == 1) {
    std::wostringstream min;
    min << wxString::FromDouble(*master_list_.cbegin());
    if (!master_unit_.empty()) {
      min << " [" << wxString::FromUTF8(master_unit_) << "]";
    }
    min_value_->SetLabel(min.str());
    max_value_->SetLabel("*");
  } else {
    std::wostringstream min;
    min << wxString::FromDouble(*master_list_.cbegin());
    if (!master_unit_.empty()) {
      min << " [" << wxString::FromUTF8(master_unit_) << "]";
    }
    min_value_->SetLabel(min.str());

    std::wostringstream max;
    max << wxString::FromDouble(*master_list_.crbegin());
    if (!master_unit_.empty()) {
      max << " [" << wxString::FromUTF8(master_unit_) << "]";
    }
    max_value_->SetLabel(max.str());
  }
  int max_index = static_cast<int>(master_list_.size()) - 1;
  master_ctrl_->SetRange(0, max_index);
}

void MasterDisplay::SetMasterValue() const {
  if (master_absolute_ == nullptr || master_relative_ == nullptr) {
    return;
  }
  if (project_ == nullptr) {
    master_absolute_->SetLabel("*");
    master_relative_->SetLabel("*");
    return;
  }

  const double master_value = project_->GetMasterReference();
  std::wostringstream value;
  value << wxString::FromDouble(master_value);
  if (!master_unit_.empty()) {
    value << " [" << wxString::FromUTF8(master_unit_) << "]";
  }
  master_relative_->SetLabel(value.str());

  uint64_t master_value_ns = project_->GetUtcBaseTime();
  if (master_value_ns == 0 || master_unit_ != "s") {
    master_absolute_->SetLabel("*");
    return;
  }
  master_value_ns += static_cast<int64_t>(1'000'000'000.0 * master_value);
  const uint64_t master_value_ms = master_value_ns / 1'000'000;

  if (master_unit_ == "s") {
    const sys_time utc_time{milliseconds(master_value_ms)};
    const zoned_time local_time(current_zone(), utc_time);
    const std::string date_time = std::format("{:L%x %T}", local_time);
    master_absolute_->SetLabel(wxString::FromUTF8(date_time));
 }
}

} // plot