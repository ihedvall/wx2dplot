/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/bmpbndl.h>
#include <wx/vector.h>

namespace plot {

enum class ConfigType : int {
  kConfig = 0,
  kDataFiles,
  kDataFile,
  kDbFile,
  kHexFile,
  kAnyFile,
  kLayers,
  kLayer,
};

class ConfigData final : public wxTreeItemData {
public:
  ConfigData() = delete;

  ConfigData(std::string name, ConfigType type)
      : name_(std::move(name)), type_(type) {}

  bool operator==(const ConfigData& rhs) const {
    return name_ == rhs.name_ && type_ == rhs.type_;
  }

  [[nodiscard]] const std::string& GetName() const { return name_; }
  [[nodiscard]] ConfigType GetType() const { return type_; }
private:
  std::string name_;
  ConfigType type_ = ConfigType::kConfig;
};

class ConfigWindow : public wxPanel {
public:
  explicit ConfigWindow(wxWindow* parent, wxWindowID id = wxID_ANY);

  void Redraw();
private:
  wxTreeCtrl* tree_ctrl_ = nullptr;
  wxVector<wxBitmapBundle> image_list_;
  void CreateImageList();
  wxDECLARE_EVENT_TABLE();
};

}  // namespace plot


