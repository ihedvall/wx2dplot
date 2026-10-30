/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>
#include <string>

#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/timer.h>

#include "plot/configuration.h"

namespace plot {

class MainFrame;

class LinePlotDocument : public wxDocument {
public:
  LinePlotDocument();
  ~LinePlotDocument() override;

  [[nodiscard]] static MainFrame* GetMainFrame();

  bool OnNewDocument() override;

  [[nodiscard]] Configuration& GetConfig() { return config_; }
  [[nodiscard]] const Configuration& GetConfig() const { return config_; }

protected:

  bool DoOpenDocument(const wxString& filename) override;
  bool DoSaveDocument(const wxString& filename) override;

private:
  Configuration orig_config_;
  Configuration config_;
  wxTimer modify_timer_;

  void OnModifyTimer(wxTimerEvent& event);

  wxDECLARE_DYNAMIC_CLASS(LinePlotDocument);
  wxDECLARE_EVENT_TABLE();
};



}
