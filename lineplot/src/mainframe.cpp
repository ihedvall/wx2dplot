/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "mainframe.h"

#include <util/timestamp.h>
#include <util/utilfactory.h>

#include <wx/aboutdlg.h>
#include <wx/config.h>
#include <wx/app.h>

#include <array>
#include <string>
#include <chrono>
#include <filesystem>

#include <boost/algorithm/string.hpp>

#include "plot/plotlogstream.h"

#include "lineplotapp.h"
#include "lineplotdocument.h"
#include "lineplotframe.h"
#include "windowid.h"

using namespace util;
using namespace util::log;
using namespace util::time;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::filesystem;
using namespace boost::algorithm;

namespace plot {

wxBEGIN_EVENT_TABLE(MainFrame, wxDocParentFrame)
  EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
  EVT_CLOSE(MainFrame::OnClose)
  EVT_DROP_FILES(MainFrame::OnDropFiles)
  EVT_TIMER(kIdMainFrameTimer, MainFrame::OnTimer)
  EVT_MENU_RANGE(kIdAppearanceSystem, kIdAppearanceLight,
    MainFrame::OnAppearance)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager* doc_manager,
                                     const wxString& title,
                                     const wxPoint& start_pos,
                                     const wxSize& start_size, bool maximized)
    : wxDocParentFrame(doc_manager, nullptr, wxID_ANY, title, start_pos,
                       start_size) {
  wxDocParentFrame::SetMinSize({-1, 600});
#ifdef _WIN32
  wxIcon app_icon("APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE);
#else
  wxIcon app{wxICON(app_icon)};
#endif
  SetIcon(app_icon);
  wxWindow::SetName("SelectionListTop");
#if (_MSC_VER)
  wxTopLevelWindowMSW::Maximize(maximized);
#else
  wxTopLevelWindowNative::Maximize(maximized);
#endif
#if (WIN32)
  wxWindow::DragAcceptFiles(true);
#endif

  // File
  auto* menu_file = new wxMenu;
  menu_file->Append(wxID_NEW);
  menu_file->Append(wxID_OPEN);
  menu_file->Append(wxID_CLOSE);
  menu_file->AppendSeparator();
  menu_file->Append(wxID_SAVE);
  menu_file->Append(wxID_SAVEAS);
  menu_file->AppendSeparator();
  menu_file->Append(wxID_EXIT);

  doc_manager->FileHistoryUseMenu(menu_file);
  if (auto* app_config = wxConfig::Get(); app_config != nullptr) {
    doc_manager->FileHistoryLoad(*app_config);
  }
  auto* menu_appearance = new wxMenu;
  menu_appearance->AppendRadioItem(kIdAppearanceSystem,
    "System Mode","Let the system choose the light or dark mode");
  menu_appearance->AppendRadioItem(kIdAppearanceDark,
    "Dark Mode", "Select the dark mode");
  menu_appearance->AppendRadioItem(kIdAppearanceLight,
    "Light Mode", "Select the light mode");

  auto& app = wxGetApp();
  switch (app.GetAppearance()) {
    case wxApp::Appearance::System:
      menu_appearance->Check(kIdAppearanceSystem, true);
      break;

    case wxApp::Appearance::Dark:
      menu_appearance->Check(kIdAppearanceDark, true);
      break;

    case wxApp::Appearance::Light:
      menu_appearance->Check(kIdAppearanceLight, true);
      break;
  }

  // ABOUT
  auto* menu_about = new wxMenu;

  menu_about->Append(kIdOpenLogFile, "Open Log File",
    "Opens the log file in the default text editor.");
  menu_about->AppendSeparator();
  menu_about->AppendSubMenu(menu_appearance, "Appearance",
    "Select the application colour scheme.");
  menu_about->AppendSeparator();
  menu_about->Append(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT),
    "Opens a information dialog that describe this application.");

  menu_bar_ = new wxMenuBar;
  menu_bar_->Append(menu_file, wxGetStockLabel(wxID_FILE));
  menu_bar_->Append(menu_about, wxGetStockLabel(wxID_HELP));
  wxFrameBase::SetMenuBar(menu_bar_);

  if (status_bar_ = wxDocParentFrame::CreateStatusBar();
      status_bar_ != nullptr) {
    // Log output, Status, Time.
    constexpr std::array<int, 2> status_width = {-1, 120};
    status_bar_->SetFieldsCount(status_width.size(), status_width.data());

    constexpr std::array<int, 2> status_styles = {wxSB_SUNKEN, wxSB_FLAT};
    status_bar_->SetStatusStyles(status_styles.size(), status_styles.data());

    status_bar_->SetStatusText("", 0);
    status_bar_->SetStatusText("Time", 1);
  }

  frame_ = new LinePlotFrame(this);
  timer_ = new wxTimer(this, kIdMainFrameTimer);
  timer_->Start(500);
}

LinePlotFrame* MainFrame::GetFrame() const {
  return frame_;
}


LinePlotDocument* MainFrame::GetDocument() {
  if (auto* doc_manager = wxDocManager::GetDocumentManager();
      doc_manager != nullptr ) {
    return dynamic_cast<LinePlotDocument*>(
                  doc_manager->GetCurrentDocument());
  }
  return nullptr;
}

void MainFrame::OnClose(wxCloseEvent &event) {
  timer_->Stop();
  // If the window is minimized. Do not save as last position
  if (!IsIconized()) {
    bool maximized = IsMaximized();
    wxPoint end_pos = GetPosition();
    wxSize end_size = GetSize();
    auto* app_config = wxConfig::Get();

    if (maximized) {
      app_config->Write("/MainWin/Max",maximized);
    } else {
      app_config->Write("/MainWin/X", end_pos.x);
      app_config->Write("/MainWin/Y", end_pos.y);
      app_config->Write("/MainWin/XWidth", end_size.x);
      app_config->Write("/MainWin/YWidth", end_size.y);
      app_config->Write("/MainWin/Max", maximized);
    }
  }
  event.Skip(true);
}

void MainFrame::OnAbout(wxCommandEvent&) {
  wxAboutDialogInfo info;
  info.SetName("Line Plot Application");
  info.SetVersion("1.0");
  info.SetDescription("View 2D line plots");

  wxArrayString devs;
  devs.push_back("Ingemar Hedvall");
  info.SetDevelopers(devs);

  info.SetCopyright("(C) 2026 Ingemar Hedvall");
  info.SetLicense("MIT License (https://opensource.org/licenses/MIT)\n"
      "Copyright 2026 Ingemar Hedvall\n"
      "\n"
      "Permission is hereby granted, free of charge, to any person obtaining a copy of this\n"
      "software and associated documentation files (the \"Software\"),\n"
      "to deal in the Software without restriction, including without limitation the rights to use, copy,\n"
      "modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,\n"
      "and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n"
      "\n"
      "The above copyright notice and this permission notice shall be included in all copies or substantial\n"
      "portions of the Software.\n"
      "\n"
      "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\n"
      "INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR\n"
      "PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,\n"
      "DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR\n"
      "IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."
  );
  if (!info.HasIcon()) {
#ifdef _WIN32
      wxIcon app("APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE);
#else
      wxIcon app {wxICON(app)};
#endif
      info.SetIcon(app);
  }
  wxAboutBox(info);
}

void MainFrame::OnAppearance(wxCommandEvent& event) {
  auto& app = wxGetApp();
  if (menu_bar_ == nullptr) {
    PLOT_ERROR() << "Menu bar is not initialized.";
    return;
  }

  wxApp::Appearance old_appearance = app.GetAppearance();
  wxApp::Appearance new_appearance = old_appearance;
  switch (event.GetId()) {
    case kIdAppearanceDark:
      new_appearance = wxApp::Appearance::Dark;
      break;

    case kIdAppearanceLight:
      new_appearance = wxApp::Appearance::Light;
      break;

    case kIdAppearanceSystem:
    default:
      new_appearance = wxApp::Appearance::System;
      break;
  }
  if (new_appearance != old_appearance) {
    if (auto* app_config = wxConfig::Get(); app_config != nullptr) {
      app_config->Write("MainWin/Appearance",
        static_cast<int>(new_appearance));
    }
    wxMessageBox(
      "You need to restart the application\n"
        "before the changes take effect.",
        "Restart the application",
        wxOK | wxCENTER | wxICON_INFORMATION);
  }
}


void MainFrame::OnDropFiles(wxDropFilesEvent& event) {
  const int file_count = event.GetNumberOfFiles();
  const wxString* file_list = event.GetFiles();
  auto* man = wxDocManager::GetDocumentManager();
  auto* doc = GetDocument();
  if (file_list == nullptr || man == nullptr)  {
    PLOT_ERROR() << "Dropping the file error. No document or manager.";
    return;
  }
  for (int index = 0; index < file_count; ++index) {
    try {
      const path fullname(file_list[index].ToStdWstring());
      if (!exists(fullname)) {
        continue;
      }

      const std::string extension = fullname.extension().string();
      if (iequals(extension, ".lab")) {
        man->CreateDocument(fullname.wstring(), wxDOC_SILENT);
      } else if (iequals(extension,".a2l") && doc != nullptr ) {
        // doc->AttachA2lFile(fullname.wstring());
      } else if (iequals(extension, ".ecuzip")) {
        // TODO: Add all available files
      }
    } catch (const std::exception& err) {
      wxMessageBox("Dropping the file error. " + wxString(err.what()));
      return;
    }
  }
}


void MainFrame::OnUpdateNoDocument(wxUpdateUIEvent& event) {
  if (const auto* doc = GetDocument();
      doc != nullptr) {
    event.Enable(true);
  } else {
    event.Enable(false);
  }
}

void MainFrame::OnTimer(wxTimerEvent& event) {
  if (event.GetId() != kIdMainFrameTimer) {
    return;
  }
  if (status_bar_ != nullptr) {
    wxString date_time;;
    const auto now = system_clock::now();
    if (auto* zone = current_zone(); zone != nullptr) {
      auto local = zone->to_local(now);
      date_time = std::format(L"{:L%x %R}", local);
    }
    const wxString old_date_time = status_bar_->GetStatusText(1);
    if (old_date_time != date_time) {
      status_bar_->SetStatusText(date_time, 1);
    }
  }
}








}  // namespace a2l