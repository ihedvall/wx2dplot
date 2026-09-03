/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "lineplotview.h"

#include <wx/docview.h>

#include <filesystem>
#include <sstream>

#include "plot/plotlogstream.h"

#include "mainframe.h"
#include "lineplotdocument.h"
#include "lineplotframe.h"
#include "lineplotapp.h"

using namespace std::filesystem;

namespace plot {

wxIMPLEMENT_DYNAMIC_CLASS(LinePlotView,wxView)

wxBEGIN_EVENT_TABLE(LinePlotView, wxView)
wxEND_EVENT_TABLE()

LinePlotDocument *LinePlotView::GetDoc() const {
  return dynamic_cast<LinePlotDocument*>(wxView::GetDocument());
}
LinePlotFrame* LinePlotView::GetFrame() {
  if (auto* parent = GetMainFrame(); parent != nullptr ) {
    return parent->GetFrame();
  }
  return nullptr;
}

MainFrame* LinePlotView::GetMainFrame() {
  const auto& app = wxGetApp();
  return dynamic_cast<MainFrame*>(app.GetTopWindow());
}

void LinePlotView::OnDraw(wxDC* dc) {

}

bool LinePlotView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate( doc,flags)) {
    return false;
  }

  if (auto* frame = GetFrame(); frame != nullptr) {
    frame->SetView(this);
  }
  UpdateTitle();
  return true;
}

bool LinePlotView::OnClose(bool del) {
  if (!wxView::OnClose(del)) {
    return false;
  }
  Activate(false);

  auto& app = wxGetApp();
  if (auto* parent = dynamic_cast<MainFrame*>(app.GetTopWindow());
      parent != nullptr ) {
    if (auto* frame = parent->GetFrame(); frame != nullptr) {
      frame->ClearBackground();
      frame->SetView(nullptr);
    }
    parent->SetTitle(app.GetAppDisplayName());
  }
  SetFrame(nullptr);
  return true;
}

void LinePlotView::OnUpdate(wxView* sender, wxObject* hint) {
  wxView::OnUpdate(sender, hint);
  UpdateTitle();
  if (auto* frame = GetFrame(); frame != nullptr) {
    frame->Redraw();
    frame->Update();
  }
}

void LinePlotView::UpdateTitle() const {
  const auto& app = wxGetApp();
  std::wostringstream title;
  title << app.GetAppDisplayName();
  if (const auto* doc = GetDoc(); doc != nullptr) {
    wxString name = doc->GetFilename();
    std::wstring short_name;
    try {
      const path fullname(name.ToStdWstring());
      const path filename = fullname.filename();
      short_name = filename.wstring();
    } catch (const std::exception& err) {
      PLOT_ERROR() << "Filename error. Error: " << err.what();
      short_name.clear();
    }
    if (short_name.empty()) {
      title << " - " << "No Name";
    } else {
      title << " - " << short_name;
    }
  }
  if (auto* main_frame = GetMainFrame(); main_frame != nullptr) {
    const wxString old_title = main_frame->GetTitle();
    if (!old_title.IsSameAs(title.str()) ) {
      main_frame->SetTitle(title.str());
    }
  }
}


}  // namespace plot