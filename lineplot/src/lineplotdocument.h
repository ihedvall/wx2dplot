/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>

#include <wx/docview.h>
#include <wx/wx.h>

#include "plot/project.h"

namespace plot {

class MainFrame;

class LinePlotDocument : public wxDocument {
public:
  LinePlotDocument() = default;

  [[nodiscard]] static MainFrame* GetMainFrame();
  [[nodiscard]] Project& GetProject() { return project_; }

  bool OnNewDocument() override;
  bool OnCloseDocument() override;

protected:

  bool DoOpenDocument(const wxString& filename) override;
  bool DoSaveDocument(const wxString& filename) override;

private:

  Project project_;

  wxDECLARE_DYNAMIC_CLASS(LinePlotDocument);
  wxDECLARE_EVENT_TABLE();
};



}
