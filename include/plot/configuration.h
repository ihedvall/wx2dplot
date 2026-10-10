/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <map>

#include "plot/layer.h"

namespace plot {

using FileList = std::map<std::string, std::wstring>;
using LayerList = std::vector<Layer>;

class Configuration {
public:
  [[nodiscard]] int GetVersion() const { return version_; }

  void SetName(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& GetName() const { return name_; }

  void SetDescription(std::string description) {
    description_ = std::move(description);
  }
  [[nodiscard]] const std::string& GetDescription() const { return description_; }

  void SetFilename(std::wstring filename) { filename_ = std::move(filename); }
  [[nodiscard]] const std::wstring& GetFilename() const { return filename_; }

  bool AddDataFile(const std::wstring& data_file);

  void AddLayer(Layer layer) { layers_.emplace_back(std::move(layer)); }

  [[nodiscard]] const LayerList& GetLayers() const { return layers_; }
  [[nodiscard]] LayerList& GetLayers() { return layers_; }
  bool ReadXmlFile();
  bool SaveXmlFile();

  void Reset();

private:
  int version_ = 1;
  std::string name_;
  std::string description_;
  std::wstring filename_;

  FileList data_files_; ///< *.mf4
  FileList db_files_; ///< *.dbc, *.a2l
  FileList hex_files_; ///< *.s19, *.hex
  FileList any_files_; ///< *.*

  std::vector<Layer> layers_;

  static std::wstring FindFile(const std::wstring& data_file);
};

}  // namespace plot


