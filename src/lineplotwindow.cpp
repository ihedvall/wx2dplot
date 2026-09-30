/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/lineplotwindow.h"

#include <filesystem>

#include <wx/sizer.h>

namespace plot {

wxIMPLEMENT_DYNAMIC_CLASS(LinePlotWindow,wxPanel)

wxBEGIN_EVENT_TABLE(LinePlotWindow, wxPanel)
wxEND_EVENT_TABLE()

LinePlotWindow::LinePlotWindow(wxWindow *parent, wxWindowID id)
    : wxPanel( parent, id) {
  main_splitter_ = new wxSplitterWindow(this);
  main_splitter_->SetMinimumPaneSize(100);

  auto* alpha_panel = new wxPanel(main_splitter_);
  alphanumeric_display_ = new AlphaNumericDisplay(alpha_panel);
  project_display_ = new ProjectDisplay(alpha_panel);

  auto* alpha_sizer = new wxBoxSizer(wxHORIZONTAL);
  alpha_sizer->Add(project_display_, 0 ,
    wxALL | wxGROW,5);
  alpha_sizer->Add(alphanumeric_display_, 1 ,
    wxALL | wxGROW,5);
  alpha_panel->SetSizer(alpha_sizer);

  auto* plot_panel = new wxPanel(main_splitter_);
  lineplot_display_ = new LinePlotDisplay(plot_panel);
  master_display_ = new MasterDisplay(plot_panel);
  legend_display_ = new LegendDisplay(plot_panel);

  auto* left_sizer = new wxBoxSizer(wxVERTICAL);
  left_sizer->Add(lineplot_display_, 1 , wxALL | wxGROW,5);
  left_sizer->Add(master_display_, 0 , wxALL | wxGROW,5);

  auto* right_sizer = new wxBoxSizer(wxHORIZONTAL);
  right_sizer->Add(legend_display_, 0 , wxALL | wxGROW,5);
  right_sizer->Add(left_sizer, 1 , wxALL | wxGROW,5);

  plot_panel->SetSizer(right_sizer);

  main_splitter_->SplitHorizontally(plot_panel,
    alpha_panel,800);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(main_splitter_, 1 , wxALL | wxGROW,0);
  SetSizer(main_sizer);
  wxPanel::Layout();

  CallAfter([this]() -> void  {
    if (main_splitter_ != nullptr && main_splitter_->IsSplit()) {
      main_splitter_->SetSashPosition(400);
      main_splitter_->Layout();
    }
  });

}

void LinePlotWindow::SetProject(Project* project) {
  const auto* old_project = project_;
  project_ = project;
  if (project_ != nullptr && old_project != project_) {
    project_->Bind(PROJECT_CHANGED, &LinePlotWindow::OnProjectChanged,
      this);
  }

  if (alphanumeric_display_ != nullptr) {
    alphanumeric_display_->SetProject(project);
  }
  if (legend_display_ != nullptr) {
    legend_display_->SetProject(project);
  }
  if (master_display_ != nullptr) {
    master_display_->SetProject(project);
  }
  if (lineplot_display_ != nullptr) {
    lineplot_display_->SetProject(project);
  }
  if (project_display_ != nullptr) {
    project_display_->SetProject(project);
  }
}


void LinePlotWindow::Redraw() const {
  if (alphanumeric_display_ != nullptr) {
    alphanumeric_display_->Redraw();
  }
  if (legend_display_ != nullptr) {
    legend_display_->Redraw();
  }
  if (master_display_ != nullptr) {
    master_display_->Redraw();
  }
  if (lineplot_display_ != nullptr) {
    lineplot_display_->Redraw();
  }
  if (project_display_ != nullptr) {
    project_display_->Redraw();
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LinePlotWindow::OnProjectChanged(wxCommandEvent&) { // NOLINT(*-make-member-function-const)
  Redraw();
}



} // end namespace plot



