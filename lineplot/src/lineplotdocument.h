/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>
#include <string>

#include <wx/docview.h>
#include <wx/wx.h>

#include "plot/project.h"

namespace plot {

class MainFrame;

class LinePlotDocument : public wxDocument {
public:
  LinePlotDocument() = default;

  [[nodiscard]] static MainFrame* GetMainFrame();

  void SetProject(std::unique_ptr<Project> project);
  [[nodiscard]] Project* GetProject() const { return project_.get(); }

  bool OnNewDocument() override;
  bool OnCloseDocument() override;

protected:

  bool DoOpenDocument(const wxString& filename) override;
  bool DoSaveDocument(const wxString& filename) override;

private:
  std::wstring filename_;
  std::unique_ptr<Project> project_;

  wxDECLARE_DYNAMIC_CLASS(LinePlotDocument);
  wxDECLARE_EVENT_TABLE();
};



}
