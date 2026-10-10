/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>
#include <string>

#include <wx/docview.h>
#include <wx/wx.h>

#include "plot/configuration.h"

namespace plot {

class MainFrame;

class LinePlotDocument : public wxDocument {
public:
  LinePlotDocument() = default;

  [[nodiscard]] static MainFrame* GetMainFrame();

  bool OnNewDocument() override;
  bool OnCloseDocument() override;

  [[nodiscard]] Configuration& GetConfig() { return config_; }
  [[nodiscard]] const Configuration& GetConfig() const { return config_; }

protected:

  bool DoOpenDocument(const wxString& filename) override;
  bool DoSaveDocument(const wxString& filename) override;

private:
  Configuration config_;

  wxDECLARE_DYNAMIC_CLASS(LinePlotDocument);
  wxDECLARE_EVENT_TABLE();
};



}
