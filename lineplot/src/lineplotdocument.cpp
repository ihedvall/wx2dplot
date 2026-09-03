/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "lineplotdocument.h"

#include <util/stringutil.h>

#include <wx/config.h>
#include <wx/progdlg.h>

#include <filesystem>
#include <atomic>
#include <thread>
#include <chrono>


#include <plot/plotlogstream.h>

#include "mainframe.h"
#include "lineplotapp.h"
#include "lineplotframe.h"

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
  if (!IsModified()) {
    return true;
  }

  project_.SetFilename(filename.ToStdWstring());
  const bool save = true; //selection_list_.SaveLabFile();
  if (save) {
    Modify(false);
  }
  return save;
}

bool LinePlotDocument::DoOpenDocument(const wxString& filename) {
  project_ = {};
  project_.SetFilename(filename.ToStdWstring());
  const bool read = true; //selection_list_.ReadLabFile();
  if (!read) {
    PLOT_ERROR() << "Error reading the project. File: " << filename;
  } else {
    PLOT_INFO() << "Opened the project. File: " << filename;
  }

  return read;
}

bool LinePlotDocument::OnCloseDocument() {
  /*
  if (IsModified() && !selection_list_.GetFileName().empty()) {
    wxString filename = selection_list_.GetFileName();
    const bool save = selection_list_.SaveLabFile();
    if (!save) {
      A2L_ERROR() << "Error saving project. File: " << filename;
    } else {
      A2L_INFO() << "Saved the project. File: " << filename;
    }
  }
  selection_list_ = {};
  if (SelectionGrid* grid = GetSelectionGrid(); grid != nullptr) {
    grid->SetSelectionList(nullptr);
  }
  Modify(false);

  const bool close = wxDocument::OnCloseDocument();
  // No views to update but the mainframes frame should be updated.
  if (auto* main_frame = GetMainFrame(); main_frame != nullptr) {
    if (auto* frame = main_frame->GetFrame(); frame != nullptr) {
      frame->Update();
    }
  }
  return close;
  */
  return true;
}

} // namespace bus