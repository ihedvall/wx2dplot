/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/docview.h>

namespace plot {


class LinePlotFrame;
class MainFrame;
class LinePlotDocument;
class LinePlotView : public wxView  {
public:
  LinePlotView() = default;
  [[nodiscard]] LinePlotDocument* GetDoc() const;
  [[nodiscard]] static LinePlotFrame* GetFrame();
  [[nodiscard]] static MainFrame* GetMainFrame();

  bool OnCreate(wxDocument* doc, long flags) override;
  bool OnClose(bool del) override;

  void OnDraw(wxDC *dc) override;
  void OnUpdate(wxView *sender, wxObject *hint) override;
private:
  void UpdateTitle() const;
  wxDECLARE_DYNAMIC_CLASS(LinePlotView);
  wxDECLARE_EVENT_TABLE();
};

}  // namespace plot

