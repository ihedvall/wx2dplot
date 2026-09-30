/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/lineplotdisplay.h"

#include <array>

#include "../include/plot/xaxis.h"
#include "../include/plot/yaxislist.h"
#include "mpinfoxtracker.h"

namespace plot {

LinePlotDisplay::LinePlotDisplay(wxWindow *parent, wxWindowID id)
  : mpWindow(parent, id) {
  mpWindow::SetMinSize({-1,100});
  SetDrawBox(false);
  SetMargins(50,20,20,20);
}

void LinePlotDisplay::SetProject(Project* project) {
  const auto* old_project = project_;
  project_ = project;
  if (project_ != nullptr && old_project != project_) {
    project_->Bind(PROJECT_CHANGED, &LinePlotDisplay::OnProjectChanged,
      this);
    project_->Bind(MASTER_CHANGED, &LinePlotDisplay::OnMasterChanged,
      this);
  }

  if (project_ == nullptr) {
    Redraw();
  }
}

void LinePlotDisplay::Redraw() {
  DelAllLayers(mpYesDelete, true);
  if (project_ != nullptr) {
    x_axis_.Init(*project_);
    y_axis_list_.Init(*project_);

    if (!y_axis_list_.IsEmpty()) {
      auto* scale_x = new mpScaleX(wxString::FromUTF8(x_axis_.Name()),
        mpALIGN_CENTERX, true, mpLabel_AUTO);
      scale_x->SetAuto(false);
      scale_x->SetCanDelete(true);
      scale_x->SetScale(x_axis_.Min(), x_axis_.Max());
      AddLayer(scale_x);

      constexpr std::array<YAxis_Align_Type, 4> y_axis_align_types = {
        mpALIGN_LEFT, mpALIGN_RIGHT, mpALIGN_BORDER_LEFT, mpALIGN_BORDER_RIGHT
      };
      for (size_t y_axis_index = 0;
          y_axis_index < y_axis_list_.NofAxis();
          ++y_axis_index) {
        const YAxis* axis = y_axis_list_.GetAxis(y_axis_index);
        if (axis == nullptr) {
          continue;
        }
        auto* y_axis = new mpScaleY(wxString::FromUTF8(axis->Name()),
          y_axis_align_types[y_axis_index % 4], true);
        y_axis->SetAuto(false);
        y_axis->SetCanDelete(true);
        y_axis->SetScale(axis->GetMin(), axis->GetMax());
        AddLayer(y_axis);
      }
    }

    auto* plotTitle = new mpTitle(wxString::FromUTF8(project_->GetName()));
    plotTitle->SetCanDelete(true);
    AddLayer(plotTitle, false, false);

    mpColourScheme& scheme = mpColourScheme::Instance();
    for (size_t signal_index = 0;
         signal_index < y_axis_list_.NofSignals();
         ++signal_index) {
      const auto* signal = y_axis_list_.GetSignal(signal_index);
      if (signal == nullptr) {
        continue;
      }
      auto* plot = GetXYSeries(signal_index,
        wxString::FromUTF8(signal->GetName()), true);
      if (plot == nullptr) {
        continue;
      }
      plot->SetName(wxString::FromUTF8(signal->GetName()));
      std::vector<double> y_values;
      std::vector<double> x_values;
      signal->SignalValues().GetValueList(y_values, x_values);
      plot->SetPen(scheme.GetLinePen(signal_index));
      plot->SetData(x_values, y_values);

    }
    // Add a legend info layer
    tracker_ = new mpInfoXTracker();
    tracker_->SetCanDelete(true);
    //tracker_->SetLocation(mpMarginTopRight);
    tracker_->ShowDraggedSeries(false);
    tracker_->EnableSeriesValues(false);
    //tracker_->SetItemMode(mpLegendSquare);
    //tracker_->SetItemDirection(mpVertical);
    tracker_->ShowSeriesValues(true);
    tracker_->SetVisible(false);
    tracker_->IndicateSeriesIntersection(true);

    tracker_->SetVisible(false);
    tracker_->SetVerticalCursorFixed(0);
    //tracker_->SetVisible(true);
    tracker_->AddXEventFunction( [&] (double x_value) {
      // Need to convert to engineering values.
      x_value = x_value * (x_axis_.Max() - x_axis_.Min()) + x_axis_.Min();
      OnXEvent(x_value);
    });
    AddLayer(tracker_, false);
  }

  UpdateAll();
  Fit();
}

void LinePlotDisplay::OnProjectChanged(wxCommandEvent& event) {
  Redraw();
  event.Skip();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void LinePlotDisplay::OnMasterChanged(wxCommandEvent& event) { // NOLINT(*-convert-member-functions-to-static)
  event.Skip();
  if (tracker_ != nullptr && project_ != nullptr) {
    double x_value = project_->GetMasterReference();
    const double x_range = x_axis_.Max() - x_axis_.Min();
    if (x_range != 0.0) {
      x_value = (x_value - x_axis_.Min())/ x_range;
    } else {
      x_value = 0.0;
    }
    tracker_->SetVisible(true);
    tracker_->SetXValue(x_value);
  }

}

void LinePlotDisplay::OnXEvent(double x_value) {
  if (project_ == nullptr) {
    return;
  }
  /*
  project_->SetMasterReference(x_value);
  wxCommandEvent changed(PLOT_MASTER_CHANGED);
  project_->ProcessEventLocally(changed);
*/
}

} // plot