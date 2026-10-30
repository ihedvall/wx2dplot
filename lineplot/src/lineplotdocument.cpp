/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "lineplotdocument.h"

#include <plot/plotlogstream.h>
#include <util/stringutil.h>
#include <wx/config.h>

#include <atomic>
#include <chrono>
#include <filesystem>

#include "plot/lineplotwindow.h"
#include "lineplotapp.h"
#include "mainframe.h"


using namespace std::filesystem;
using namespace util::string;
using namespace std::chrono_literals;

namespace plot {
wxIMPLEMENT_DYNAMIC_CLASS(LinePlotDocument, wxDocument)

wxBEGIN_EVENT_TABLE(LinePlotDocument, wxDocument)
wxEND_EVENT_TABLE()

LinePlotDocument::LinePlotDocument() {
  modify_timer_.Bind(wxEVT_TIMER, &LinePlotDocument::OnModifyTimer, this);
  modify_timer_.Start(1000);
}

LinePlotDocument::~LinePlotDocument() {
  if (modify_timer_.IsRunning()) {
    modify_timer_.Stop();
    modify_timer_.Unbind(wxEVT_TIMER, &LinePlotDocument::OnModifyTimer, this);
  }
}

MainFrame* LinePlotDocument::GetMainFrame() {
  return dynamic_cast<MainFrame*>(wxGetApp().GetTopWindow());
}

bool LinePlotDocument::OnNewDocument() {
  const bool new_doc = wxDocument::OnNewDocument( );
  if (!new_doc) {
    return false;
  }
  UpdateAllViews();
  return true;
}

bool LinePlotDocument::DoSaveDocument(const wxString& filename) {
  config_.SetFilename(filename.ToStdWstring());
  const bool save = config_.SaveXmlFile();
  if (save) {
    orig_config_ = config_;
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
    orig_config_ = config_;
  }
  SetFilename(filename, true);
  if (MainFrame* main_frame = GetMainFrame(); main_frame != nullptr) {
    main_frame->SetTitle(filename);
    main_frame->Redraw();
  }
  return read;
}

void LinePlotDocument::OnModifyTimer(wxTimerEvent&) {
  Modify(config_ != orig_config_);
}

} // namespace bus