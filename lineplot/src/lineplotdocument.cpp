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

#include "../../include/plot/lineplotwindow.h"
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

void LinePlotDocument::SetProject(std::unique_ptr<Project> project) {
  project_ = std::move(project);
  UpdateAllViews();
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
  if (!project_ || !IsModified()) {
    return true;
  }

  filename_ = filename.ToStdWstring();
  const bool save = true; //selection_list_.SaveLabFile();
  if (save) {
    Modify(false);
  }
  return save;
}

bool LinePlotDocument::DoOpenDocument(const wxString& filename) {
  project_.reset();
  project_ = std::make_unique<Project>();
  filename_ = filename.ToStdWstring();
  const bool read = true; //selection_list_.ReadLabFile();
  if (!read) {
    PLOT_ERROR() << "Error reading the project. File: " << filename;
  } else {
    PLOT_INFO() << "Opened the project. File: " << filename;
  }

  return read;
}

bool LinePlotDocument::OnCloseDocument() {

  return true;
}

} // namespace bus