/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

// ReSharper disable CppUnusedIncludeDirective
#include "configwindow.h"

#include <filesystem>
#include <sstream>

#include <wx/sizer.h>

#include "mainframe.h"
#include "lineplotdocument.h"

#include "img/config.cpp"
#include "img/layers.cpp"
#include "img/lineplot.cpp"
#include "img/folder_close.cpp"
#include "img/folder_open.cpp"
#include "img/mf4.cpp"
#include "img/document_text.cpp"

using namespace std::filesystem;
namespace {
  constexpr int kBmpConfig = 0;
  constexpr int kBmpLayers = 1;
  constexpr int kBmpLinePlot = 2;
  constexpr int kBmpFolderClose = 3;
  constexpr int kBmpFolderOpen = 4;
  constexpr int kBmpMf4 = 5;
  constexpr int kBmpDocumentText = 6;
}

namespace plot {
wxBEGIN_EVENT_TABLE(ConfigWindow, wxPanel)
wxEND_EVENT_TABLE()



ConfigWindow::ConfigWindow(wxWindow* parent, wxWindowID id)
  : wxPanel(parent, id) {
  CreateImageList();
  tree_ctrl_ = new wxTreeCtrl(this, wxID_ANY);
  tree_ctrl_->SetImages(image_list_);


  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(tree_ctrl_, 1, wxEXPAND);
  SetSizerAndFit(main_sizer);

  Redraw();
}

void ConfigWindow::Redraw() const {
  if (tree_ctrl_ == nullptr) {
    return;
  }
  const wxTreeItemId selected = tree_ctrl_->GetSelection();
  ConfigData* selected_data = nullptr;
  if (selected.IsOk()) {
    selected_data = dynamic_cast<ConfigData*>(tree_ctrl_->GetItemData(selected));
  }
  tree_ctrl_->DeleteAllItems();

  const auto* doc = MainFrame::GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto& config = doc->GetConfig();

  const wxString name = config.GetName().empty() ? wxString("Configuration")
      : wxString::FromUTF8(config.GetName());
  auto* root_data = new ConfigData(config.GetName(), ConfigType::kConfig);
  wxTreeItemId root = tree_ctrl_->AddRoot(name,
    kBmpConfig, kBmpConfig, root_data);
  RedrawLayers(root);
  RedrawDataFiles(root);
  tree_ctrl_->ExpandAll();

  if (selected_data != nullptr && *selected_data == *root_data) {
    tree_ctrl_->SelectItem(root);
  }
}

void ConfigWindow::CreateImageList() {
  wxBitmapBundle config_bmp(wxBITMAP_PNG(config));
  wxBitmapBundle layers_bmp(wxBITMAP_PNG(layers));
  wxBitmapBundle lineplot_bmp(wxBITMAP_PNG(lineplot));
  wxBitmapBundle folder_close_bmp(wxBITMAP_PNG(folder_close));
  wxBitmapBundle folder_open_bmp(wxBITMAP_PNG(folder_open));
  wxBitmapBundle mf4_bmp(wxBITMAP_PNG(mf4));
  wxBitmapBundle decument_text_bmp(wxBITMAP_PNG(document_text));
  image_list_.emplace_back(std::move(config_bmp));
  image_list_.emplace_back(std::move(layers_bmp));
  image_list_.emplace_back(std::move(lineplot_bmp));
  image_list_.emplace_back(std::move(folder_close_bmp));
  image_list_.emplace_back(std::move(folder_open_bmp));
  image_list_.emplace_back(std::move(mf4_bmp));
  image_list_.emplace_back(std::move(decument_text_bmp));
}

void ConfigWindow::RedrawLayers(const wxTreeItemId& root) const {
  const auto* doc = MainFrame::GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto& config = doc->GetConfig();
  auto* layers_data = new ConfigData({}, ConfigType::kLayers);
  wxTreeItemId layers_root = tree_ctrl_->AppendItem(root,"Layers",
    kBmpLayers, kBmpLayers, layers_data);
  int layer_index = 1;
  for (const Layer& layer : config.GetLayers()) {
    const wxString layer_name = layer.GetName().empty() ?
         wxString("Layer" + std::to_string(layer_index))
        : wxString::FromUTF8(layer.GetName());
    auto* layer_data = new ConfigData(layer.GetName(), ConfigType::kLayer);
    tree_ctrl_->AppendItem(layers_root, layer_name,
      kBmpLinePlot, kBmpLinePlot, layer_data);
    ++layer_index;
  }
}

void ConfigWindow::RedrawDataFiles(const wxTreeItemId& root) const {
  const auto* doc = MainFrame::GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto& config = doc->GetConfig();
  auto* folder_data = new ConfigData({}, ConfigType::kDataFiles);
  wxTreeItemId files_root = tree_ctrl_->AppendItem(root,"Data Files",
    kBmpFolderClose, kBmpFolderOpen, folder_data);

  for (const std::wstring& file : config.GetDataFiles()) {
    try {
      const path fullname(file);
      const std::wstring filename = fullname.filename().wstring();
      const std::wstring parent_path = fullname.parent_path().wstring();
      auto* file_data = new ConfigData(filename, ConfigType::kDataFile);
      std::wstringstream label;
      label << filename << " (" << parent_path << ")";
      tree_ctrl_->AppendItem(files_root, label.str(),
      kBmpMf4, kBmpMf4, file_data);
    } catch (const std::exception& e) {
      PLOT_ERROR() << "Error processing file: " << e.what();
    }
  }
}

} // plot