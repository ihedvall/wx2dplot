/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/masterplotdisplay.h"

#include "../include/plot/xaxis.h"
#include "../include/plot/yaxislist.h"
#include "mpinfoxtracker.h"

namespace plot {
MasterPlotDisplay::MasterPlotDisplay(wxWindow *parent, wxWindowID id)
  : mpWindow(parent, id) {
  mpWindow::SetMinSize({-1,70});
  SetSize({-1,70});
  SetDrawBox(false);
  SetMargins(20,20,20,20);
  UnbindEvents();
}

void MasterPlotDisplay::SetProject(Project* project) {
  const auto* old_project = project_;
  project_ = project;
  if (project_ != nullptr && old_project != project_) {
    project_->Bind(PROJECT_CHANGED, &MasterPlotDisplay::OnProjectChanged,
      this);
    project_->Bind(MASTER_CHANGED, &MasterPlotDisplay::OnMasterChanged,
      this);
  }

  if (project_ == nullptr) {
    Redraw();
  }
}

void MasterPlotDisplay::Redraw() {
  const auto& scheme = mpColourScheme::Instance();
  DelAllLayers(mpYesDelete, true);
  if (project_ != nullptr) {
    x_axis_.Init(*project_);
    YAxisList y_axis_list(*project_);

    auto* scale_x = new mpScaleX(wxString::FromUTF8(x_axis_.Name()),
      mpALIGN_BORDER_TOP, true, mpLabel_AUTO);
    scale_x->SetAuto(false);
    scale_x->SetCanDelete(true);
    scale_x->SetScale(x_axis_.Min(), x_axis_.Max());
    AddLayer(scale_x);

    for (size_t y_axis_index = 0;
            y_axis_index < y_axis_list.NofAxis();
            ++y_axis_index) {
      const YAxis* axis = y_axis_list.GetAxis(y_axis_index);
      if (axis == nullptr) {
        continue;
      }
      auto* y_axis = new mpScaleY(wxEmptyString,mpALIGN_LEFT, false);
      y_axis->SetAuto(false);
      y_axis->SetCanDelete(true);
      y_axis->SetScale(0.0, 0.0);
      y_axis->SetPen(scheme.GetDefaultBgColour());
      AddLayer(y_axis);
      break;
            }

    for (size_t signal_index = 0;
         signal_index < y_axis_list.NofSignals();
         ++signal_index) {
      const auto* signal = y_axis_list.GetSignal(signal_index);
      if (signal == nullptr) {
        continue;
      }
      auto* plot = GetXYSeries(signal_index,wxEmptyString, true);
      if (plot == nullptr) {
        continue;
      }
      plot->SetName(wxEmptyString);
      std::vector<double> y_values;
      std::vector<double> x_values;
      signal->SignalValues().GetValueList(y_values, x_values);
      y_values.assign(y_values.size(), 0.0);
      plot->SetPen( wxPen(scheme.GetDefaultBgColour()) );// scheme.GetAxisColour());
      plot->SetData(x_values, y_values);
      break;
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
    tracker_->SetVisible(true);
    tracker_->IndicateSeriesIntersection(true);
    tracker_->SetVerticalCursorFixed(0);
    tracker_->AddXEventFunction( [&] (double x_value) {
      OnXEvent(x_value);
    });
    AddLayer(tracker_, false);
  }

  UpdateAll();
  Fit();
}
void MasterPlotDisplay::OnMouseMove(wxMouseEvent& event) {
    // The current mouse position
  m_mousePos = event.GetPosition();

  // Check if the user has defined a special action for this mouse event
  if (CheckUserMouseAction(event))
    return;

  bool requestRefresh = false;
  bool requestUpdateAll = false;


  // Check series values shall be shown in info legend
  if (m_InfoLegend && m_InfoLegend->IsVisible())
  {
    if ((m_InfoLegend->SeriesValuesShouldBeShown(m_PlotArea, m_mousePos)) ||
        (m_InfoLegend->GetVerticalCursorMode() == mpInfoLegend::CursorMode_fixed)              )
    {
      m_InfoLegend->ShowSeriesValues(true);
      requestRefresh = true;
    }
    else if (m_InfoLegend->IsSeriesValuesShown())
    {
      m_InfoLegend->ShowSeriesValues(false);
      requestUpdateAll = true;
    }
  }

  if(requestUpdateAll)
  {
    UpdateAll();
  }
  else if (requestRefresh)
  {
    // Calling Refresh() without setting m_cacheDirty and without going through UpdateAll() results
    // in a very lightweight and quick OnPaint event where only mouse-related overlays are rendered
    Refresh();
  }

  event.Skip();

}

void MasterPlotDisplay::UnbindEvents()
{
  auto* mp_win = dynamic_cast<mpWindow*>(this);
  // General events
  // Bind(wxEVT_PAINT, &MasterPlotDisplay::OnMpPaint, this);
  // Bind(wxEVT_SIZE, &MasterPlotDisplay::OnSize, this);
  Unbind(wxEVT_SCROLLWIN_THUMBTRACK, &MasterPlotDisplay::OnScrollThumbTrack, mp_win);
  Unbind(wxEVT_SCROLLWIN_PAGEUP, &MasterPlotDisplay::OnScrollPageUp, mp_win);
  Unbind(wxEVT_SCROLLWIN_PAGEDOWN, &MasterPlotDisplay::OnScrollPageDown, mp_win);
  Unbind(wxEVT_SCROLLWIN_LINEUP, &MasterPlotDisplay::OnScrollLineUp, mp_win);
  Unbind(wxEVT_SCROLLWIN_LINEDOWN, &MasterPlotDisplay::OnScrollLineDown, mp_win);
  Unbind(wxEVT_SCROLLWIN_TOP, &MasterPlotDisplay::OnScrollTop, mp_win);
  Unbind(wxEVT_SCROLLWIN_BOTTOM, &MasterPlotDisplay::OnScrollBottom, mp_win);

  // Mouse events
  Unbind(wxEVT_MIDDLE_UP, &MasterPlotDisplay::OnShowPopupMenu, mp_win);
  Unbind(wxEVT_RIGHT_DOWN, &MasterPlotDisplay::OnMouseRightDown, mp_win);
  Unbind(wxEVT_RIGHT_UP, &MasterPlotDisplay::OnShowPopupMenu, mp_win);
  Unbind(wxEVT_MOUSEWHEEL, &MasterPlotDisplay::OnMouseWheel, mp_win);
  // Unbind(wxEVT_MOTION, &MasterPlotDisplay::OnMouseMove, mp_win);
  // Unbind(wxEVT_LEAVE_WINDOW, &MasterPlotDisplay::OnMouseLeave, mp_win);
  // Unbind(wxEVT_LEFT_DOWN, &MasterPlotDisplay::OnMouseLeftDown, mp_win);
  // Unbind(wxEVT_LEFT_UP, &MasterPlotDisplay::OnMouseLeftRelease, mp_win);

  // Popup menu events
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnCenter, mp_win, mpID_CENTER);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnFit, mp_win, mpID_FIT);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnToggleGrids, mp_win, mpID_TOGGLE_GRID);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnToggleCoords, mp_win, mpID_TOGGLE_COORD);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnScreenShot, mp_win, mpID_SCREENSHOT);
#if defined(MP_ENABLE_CONFIG) || defined(ENABLE_MP_CONFIG)
  Unbind(wxEVT_MENU, &mpWindow::OnConfiguration, mp_win, mpID_CONFIG);
#endif // MP_ENABLE_CONFIG
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnLoadFile, mp_win, mpID_LOAD_FILE);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnZoomIn, mp_win, mpID_ZOOM_IN);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnZoomOut, mp_win, mpID_ZOOM_OUT);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnLockAspect, mp_win, mpID_LOCKASPECT);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnMouseHelp, mp_win, mpID_HELP_MOUSE);
  Unbind(wxEVT_MENU, &MasterPlotDisplay::OnFullScreen, mp_win, mpID_FULLSCREEN);
}

void MasterPlotDisplay::OnProjectChanged(wxCommandEvent& event) {
  Redraw();
  event.Skip();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void MasterPlotDisplay::OnMasterChanged(wxCommandEvent& event) { // NOLINT(*-convert-member-functions-to-static)
  event.Skip();
  if (tracker_ != nullptr && project_ != nullptr) {
    double x_value = project_->GetMasterReference();
    const double x_range = x_axis_.Max() - x_axis_.Min();
    if (x_range != 0.0) {
      x_value = (x_value - x_axis_.Min())/ x_range;
    } else {
      x_value = 0.0;
    }
    tracker_->SetXValue(x_value);
  }

}

void MasterPlotDisplay::OnXEvent(double x_value) const {
  if (project_ == nullptr) {
    return;
  }
  project_->SetMasterReference(x_value);
  wxCommandEvent changed(PLOT_MASTER_CHANGED);
  project_->ProcessEventLocally(changed);

}

}
