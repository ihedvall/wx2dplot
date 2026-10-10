/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "configwindow.h"

#include <wx/sizer.h>

#include "mainframe.h"
#include "lineplotdocument.h"

#include "img/config.cpp"
#include "img/layers.cpp"
#include "img/lineplot.cpp"

namespace {
  constexpr int kBmpConfig = 0;
  constexpr int kBmpLayers = 1;
  constexpr int kBmpLinePlot = 2;
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

void ConfigWindow::Redraw() {
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

  if (const auto& layers = config.GetLayers(); !layers.empty()) {
    auto* layers_data = new ConfigData(config.GetName(), ConfigType::kLayers);
    wxTreeItemId layers_rool = tree_ctrl_->AppendItem(root,"Layers",
    kBmpLayers, kBmpLayers, layers_data);
    int layer_index = 1;
    for (const auto& layer : layers) {
      const wxString layer_name = layer.GetName().empty() ?
           wxString("Layer" + std::to_string(layer_index))
          : wxString::FromUTF8(layer.GetName());
      auto* layer_data = new ConfigData(layer.GetName(), ConfigType::kLayer);
      tree_ctrl_->AppendItem(layers_rool, layer_name,
        kBmpLinePlot, kBmpLinePlot, layer_data);
      ++layer_index;
    }
  }

  tree_ctrl_->ExpandAll();

  if (selected_data != nullptr && *selected_data == *root_data) {
    tree_ctrl_->SelectItem(root);
  }



}

void ConfigWindow::CreateImageList() {
  wxBitmapBundle config_bmp(wxBITMAP_PNG(config));
  wxBitmapBundle layers_bmp(wxBITMAP_PNG(layers));
  wxBitmapBundle lineplot_bmp(wxBITMAP_PNG(lineplot));
  image_list_.emplace_back(std::move(config_bmp));
  image_list_.emplace_back(std::move(layers_bmp));
  image_list_.emplace_back(std::move(lineplot_bmp));
}

} // plot