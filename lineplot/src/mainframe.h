/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <wx/wx.h>
#include <wx/docview.h>
#include <wx/timer.h>

namespace plot {

class LinePlotDocument;
class LinePlotWindow;
class MainFrame : public wxDocParentFrame {
 public:
  MainFrame(wxDocManager* doc_manager,
            const wxString& title,
            const wxPoint& start_pos,
            const wxSize& start_size,
            bool maximized);

  [[nodiscard]] LinePlotWindow* GetFrame() const;
  [[nodiscard]] static LinePlotDocument* GetDocument();
  void SetTitle(const wxString& title) override;
 private:

  LinePlotWindow* frame_ = nullptr;
  wxMenuBar* menu_bar_ = nullptr;
  wxStatusBar* status_bar_ = nullptr;
  wxTimer* timer_ = nullptr;

  void OnClose(wxCloseEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnAppearance(wxCommandEvent& event);
  void OnGraphColor(wxCommandEvent& event);
  void OnDropFiles(wxDropFilesEvent& event);
  void OnSinusProject(wxCommandEvent& event);

  void OnUpdateEmptyProject(wxUpdateUIEvent& event);
  void OnUpdateNoDocument(wxUpdateUIEvent& event);
  void OnTimer(wxTimerEvent& event);

  wxDECLARE_EVENT_TABLE();
};

} // end namespace bus

