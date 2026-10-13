/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "lineplotdocument.h"

#include <plot/plotlogstream.h>
#include <util/stringutil.h>
#include <wx/config.h>
#include <wx/progdlg.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <thread>

#include "plot/lineplotwindow.h"
#include "lineplotapp.h"
#include "mainframe.h"
#include "windowid.h"

using namespace std::filesystem;
using namespace util::string;
using namespace std::chrono_literals;

namespace plot {
wxIMPLEMENT_DYNAMIC_CLASS(LinePlotDocument, wxDocument)

wxBEGIN_EVENT_TABLE(LinePlotDocument, wxDocument)
wxEND_EVENT_TABLE()

MainFrame* LinePlotDocument::GetMainFrame() {
  return dynamic_cast<MainFrame*>(wxGetApp().GetTopWindow());
}

bool LinePlotDocument::OnNewDocument() {
  const bool new_doc = wxDocument::OnNewDocument( );
  if (!new_doc) {
    return false;
  }
  Modify(true);
  UpdateAllViews();
  return true;
}

bool LinePlotDocument::DoSaveDocument(const wxString& filename) {
  config_.SetFilename(filename.ToStdWstring());
  const bool save = config_.SaveXmlFile();
  if (save) {
    Modify(false);
  }
  SetFilename(filename, true);
  if (MainFrame* main_frame = GetMainFrame(); main_frame != nullptr) {
    main_frame->SetTitle(filename);
  }
  return save;
}

bool LinePlotDocument::DoOpenDocument(const wxString& filename) {
  config_.Reset();
  config_.SetFilename(filename.ToStdWstring());
  const bool read = config_.ReadXmlFile();
  if (!read) {
    PLOT_ERROR() << "Error reading the project. File: " << filename;
  } else {
    PLOT_INFO() << "Opened the project. File: " << filename;
  }
  SetFilename(filename, true);
  if (MainFrame* main_frame = GetMainFrame(); main_frame != nullptr) {
    main_frame->SetTitle(filename);
    main_frame->Redraw();
  }
  return read;
}

bool LinePlotDocument::OnCloseDocument() {

  return true;
}

} // namespace bus