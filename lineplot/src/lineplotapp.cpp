/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "lineplotapp.h"

#include <source_location>
#include <locale>
#include <filesystem>

#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/docview.h>

#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>

#include <util/logconfig.h>
#include <util/logstream.h>

#include "plot/plotlogstream.h"

#include "mainframe.h"
#include "lineplotdocument.h"
#include "lineplotview.h"
#include "windowid.h"

using namespace util::log;
using namespace std::filesystem;
using namespace boost::algorithm;

wxIMPLEMENT_APP(plot::LinePlotApp); // NOLINT(*-pro-type-static-cast-downcast)

namespace {
constexpr wxCmdLineEntryDesc kCmdLineDesc[2] = {
  {wxCMD_LINE_PARAM,nullptr,nullptr,
   "input file", wxCMD_LINE_VAL_STRING,
   wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
   wxCMD_LINE_DESC_END
 };

void PlotLogFunction(const std::source_location& location,
                 plot::PlotLogSeverity severity,
                 const std::string& message) {
  LogMessage msg;
  msg.message = message;
  msg.severity = static_cast<LogSeverity>(severity);
  msg.line = location.line();
  msg.column = location.column();
  msg.file = location.file_name();
  msg.function = location.function_name();
  LogConfig::Instance().AddLogMessage(msg);
}

}

namespace plot {

wxBEGIN_EVENT_TABLE(LinePlotApp, wxApp)
  EVT_UPDATE_UI(kIdOpenLogFile, LinePlotApp::OnUpdateOpenLogFile)
  EVT_MENU(kIdOpenLogFile, LinePlotApp::OnOpenLogFile)
wxEND_EVENT_TABLE()

bool LinePlotApp::OnInit() {
  if (!wxApp::OnInit()) {
    return false;
  }

  wxInitAllImageHandlers();
  // Setup correct localization when formatting date and times
  std::locale::global(std::locale(""));
  std::wcout.imbue(std::locale());

    // Setup system basic configuration
  SetVendorDisplayName("IH Development");
  SetVendorName("IH_Development");
  SetAppName("LinePlot");
  SetAppDisplayName("Line Plot");

  // Set up the log file.
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("lineplot/log");
  log_config.BaseName("lineplot");
  log_config.CreateDefaultLogger();

  PlotLogStream::SetLogFunction(::PlotLogFunction);
  LOG_INFO() << "Log File created. Path: " << log_config.GetLogFile();

  notepad_ = FindNotepad();

  int appearance = static_cast<int>(Appearance::System);
  wxPoint start_pos(wxDefaultPosition);
  wxSize start_size(wxDefaultSize);
  bool maximized = false;
  if (auto* app_config = wxConfig::Get(); app_config != nullptr) {
    appearance_ = static_cast<Appearance>(app_config->Read(
              "MainWin/Appearance", appearance));

    app_config->Read("/MainWin/X",&start_pos.x, wxDefaultPosition.x);
    app_config->Read("/MainWin/Y",&start_pos.y, wxDefaultPosition.x);

    app_config->Read("/MainWin/XWidth",&start_size.x, 1200);
    app_config->Read("/MainWin/YWidth",&start_size.y, 800);

    app_config->Read("/MainWin/Max",&maximized, maximized);
  }

  AppearanceResult result = SetAppearance(appearance_);
  if (result == AppearanceResult::Failure) {
    PLOT_ERROR() << "Failed to set the appearance. Appearance: " << appearance;
  } else if (result == AppearanceResult::CannotChange) {
    PLOT_INFO() << "Cannot change appearance. Appearance: " << appearance;
  }

  auto* doc_manager = new wxDocManager;
  new wxDocTemplate(doc_manager, "Label File","*.lab;",
          "",  "lab",
   "Label Document","Line Plot View",
                     wxCLASSINFO(LinePlotDocument),
                     wxCLASSINFO(LinePlotView));
  doc_manager->SetMaxDocsOpen(1);

  auto* main_frame = new MainFrame(doc_manager, GetAppDisplayName(), start_pos,
                              start_size, maximized);
  SetTopWindow(main_frame);
  main_frame->Show(true);


  if (!label_file_.empty()) {
    doc_manager->CreateDocument(label_file_);
  }
  if (LinePlotDocument* doc = MainFrame::GetDocument();
    doc != nullptr && !a2l_file_.empty()) {
    // doc->AttachA2lFile(a2l_file_);
    doc->UpdateAllViews();
  }

  return true;
}

int LinePlotApp::OnExit() {
  LOG_INFO() << "Closing app";


  auto* app_config = wxConfig::Get();
  if (auto* doc_manager = wxDocManager::GetDocumentManager();
      doc_manager != nullptr && app_config != nullptr) {
    doc_manager->FileHistorySave(*app_config);
    delete doc_manager;
  }
  LOG_INFO() << "Saved file history.";

  PlotLogStream::ResetLogFunction();

  auto& log_config = LogConfig::Instance();
  log_config.DeleteLogChain();

  return wxApp::OnExit();
}

void LinePlotApp::OnOpenLogFile(wxCommandEvent& event) {
  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  OpenFile(logfile);

}

// ReSharper disable once CppMemberFunctionMayBeConst
void LinePlotApp::OnUpdateOpenLogFile(wxUpdateUIEvent &event) {
  if (notepad_.empty()) {
    event.Enable(false);
    return;
  }

  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  try {
    std::filesystem::path p(logfile);
    const bool exist = std::filesystem::exists(p);
    event.Enable(exist);
  } catch (const std::exception&) {
    event.Enable(false);
  }
}


void LinePlotApp::OpenFile(const std::string& filename) {
  if (!notepad_.empty()) {
    std::vector<std::string> arguments = {filename};
    boost::process::process open_log(ctx_, notepad_.c_str(), arguments);
    open_log.detach();
  }
}

void LinePlotApp::OnInitCmdLine(wxCmdLineParser& parser) {
  wxApp::OnInitCmdLine(parser);
  parser.SetDesc(kCmdLineDesc);
  parser.SetCmdLine(argc, argv);

}

bool LinePlotApp::OnCmdLineParsed(wxCmdLineParser& parser) {
  if (!wxApp::OnCmdLineParsed(parser)) {
    return false;
  }

  for ( size_t input = 0; input < parser.GetParamCount(); ++input) {
    const wxString inputFile = parser.GetParam(input);
    try {
      path fullname(inputFile.ToStdWstring());
      if (!exists(fullname)) {
        PLOT_ERROR() << "File does not exist: " << inputFile;
        continue;
      }
      const std::string extension = fullname.extension().string();
      if (iequals(extension ,".lab")) {
        label_file_ = inputFile.ToStdWstring();
      } else if (iequals(extension ,".a2l")) {
        a2l_file_ = inputFile.ToStdWstring();
      }
    }
    catch (const std::exception& err) {
      PLOT_ERROR() << "Error opening file: " << err.what();
    }
  }

  return true;
}

} // plot