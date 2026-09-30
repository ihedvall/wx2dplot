/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

// ReSharper disable CppMemberFunctionMayBeConst
#include "plot/projectdisplay.h"

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/button.h>

namespace plot {

wxBEGIN_EVENT_TABLE(ProjectDisplay, wxPanel)
wxEND_EVENT_TABLE()

ProjectDisplay::ProjectDisplay(wxWindow* parent, wxWindowID id)
  : wxPanel(parent, id),
    timer_(this) {
  wxPanel::SetMinSize(wxSize(350, -1));

  auto* name_label = new wxStaticText(this, wxID_ANY, "Name:");
  name_ctrl_ = new wxTextCtrl(this, wxID_ANY);
  name_ctrl_->SetMinSize(wxSize(200, -1));
  name_ctrl_->Enable(false);

  auto* desc_label = new wxStaticText(this, wxID_ANY,
    "Description:");
  desc_ctrl_ = new wxTextCtrl(this, wxID_ANY);
  desc_ctrl_->SetMinSize(wxSize(200, -1));
  desc_ctrl_->Enable(false);

  x_zero_ctrl_ = new wxCheckBox(this, wxID_ANY,
    "Include Zero in X Axis");
  x_zero_ctrl_->Bind(wxEVT_CHECKBOX, &ProjectDisplay::OnXZero, this);
  x_zero_ctrl_->Enable(false);

  y_zero_ctrl_ = new wxCheckBox(this, wxID_ANY,
    "Include Zero in Y Axis");
  y_zero_ctrl_->Bind(wxEVT_CHECKBOX, &ProjectDisplay::OnYZero, this);
  y_zero_ctrl_->Enable(false);

  apply_button_ = new wxButton(this, wxID_ANY, "Apply Changes");

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(name_label, 0,  wxALL, 5);
  main_sizer->Add(name_ctrl_, 0,  wxALL, 5);
  main_sizer->Add(desc_label, 0,  wxALL, 5);
  main_sizer->Add(desc_ctrl_, 0,  wxALL | wxGROW, 5);
  main_sizer->Add(x_zero_ctrl_, 0,  wxALL | wxGROW, 5);
  main_sizer->Add(y_zero_ctrl_, 0,  wxALL | wxGROW, 5);
  main_sizer->Add(apply_button_, 0,  wxALL | wxGROW, 5);

  apply_button_->Bind(wxEVT_BUTTON, &ProjectDisplay::OnApply, this);

  Bind(wxEVT_TIMER, &ProjectDisplay::OnTimer, this);
  timer_.Start(500);

  SetSizer(main_sizer);
}

ProjectDisplay::~ProjectDisplay() {
  if (timer_.IsRunning()) {
    timer_.Stop();
  }
}

void ProjectDisplay::SetProject(Project* project) {
  Project* old_project = project_;
  project_ = project;

  if (old_project != project_) {
      Redraw();
  }
}

void ProjectDisplay::Redraw() {
  if (name_ctrl_ == nullptr || desc_ctrl_ == nullptr) {
    return;
  }
  if (project_ == nullptr) {
    name_ctrl_->Enable(false);
    name_ctrl_->ChangeValue("");

    desc_ctrl_->Enable(false);
    desc_ctrl_->ChangeValue("");

    x_zero_ctrl_->Enable(false);
    y_zero_ctrl_->Enable(false);
  } else {
    name_ctrl_->Enable(true);
    name_ctrl_->ChangeValue(wxString::FromUTF8(project_->GetName()));

    desc_ctrl_->Enable(true);
    desc_ctrl_->ChangeValue(wxString::FromUTF8(project_->GetDescription()));

    x_zero_ctrl_->Enable(true);
    y_zero_ctrl_->Enable(true);
  }
  Refresh();
}


void ProjectDisplay::OnApply(wxCommandEvent&) {
  if (name_ctrl_ == nullptr || desc_ctrl_ == nullptr) {
    return;
  }

  name_ctrl_->SetModified(false);
  desc_ctrl_->SetModified(false);
  apply_button_->Enable(false);
  if (project_ != nullptr) {
    const std::string name = name_ctrl_->GetValue().utf8_string();
    project_->SetName(name);
    const std::string desc = desc_ctrl_->GetValue().utf8_string();
    project_->SetDescription(desc);
    wxCommandEvent changed(PROJECT_CHANGED);
    project_->ProcessEventLocally(changed);
  }
}

void ProjectDisplay::OnTimer(wxTimerEvent&) {
  if (apply_button_ == nullptr) {
    return;
  }
  if (name_ctrl_ == nullptr || desc_ctrl_ == nullptr) {
    apply_button_->Enable(false);
    return;
  }

  if (project_ == nullptr) {
    apply_button_->Enable(false);
  } else {
    const bool modified = name_ctrl_->IsModified() || desc_ctrl_->IsModified();
    apply_button_->Enable(modified);
  }
}

void ProjectDisplay::OnXZero(wxCommandEvent& event) {
  if (project_ == nullptr) {
    return;
  }
  const bool checked = event.IsChecked();
  project_->SetIncludeZeroX(checked);
  wxCommandEvent changed(PROJECT_CHANGED);
  project_->ProcessEventLocally(changed);
}

void ProjectDisplay::OnYZero(wxCommandEvent& event) {
  if (project_ == nullptr) {
    return;
  }
  const bool checked = event.IsChecked();
  project_->SetIncludeZeroY(checked);
  wxCommandEvent changed(PROJECT_CHANGED);
  project_->ProcessEventLocally(changed);
}
} // plot