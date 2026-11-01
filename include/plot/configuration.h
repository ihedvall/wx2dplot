/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <wx/xml/xml.h>

#include "plot/layer.h"

namespace plot {

using FileList = std::vector<std::wstring>;
using LayerList = std::vector<Layer>;

class Configuration {
public:
  Configuration();
  Configuration(const Configuration& config);
  Configuration& operator = (const Configuration& config);
  bool operator == (const Configuration& config) const;


  [[nodiscard]] int GetVersion() const { return version_; }

  void SetName(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& GetName() const { return name_; }

  void SetDescription(std::string description) {
    description_ = std::move(description);
  }
  [[nodiscard]] const std::string& GetDescription() const { return description_; }

  void SetFilename(std::wstring filename) { filename_ = std::move(filename); }
  [[nodiscard]] const std::wstring& GetFilename() const { return filename_; }

  static std::wstring_view GetFileWildcardFilter();
  bool AddFile(const std::wstring& file);
  void DelFile(const std::wstring& file);

  [[nodiscard]] const FileList& GetDataFiles() const { return data_files_; }
  [[nodiscard]] const FileList& GetDbFiles() const { return db_files_; }
  [[nodiscard]] const FileList& GetHexFiles() const { return hex_files_; }
  [[nodiscard]] const FileList& GetLabFiles() const { return lab_files_; }
  [[nodiscard]] const FileList& GetAnyFiles() const { return any_files_; }

  void AddLayer(Layer layer) { layers_.emplace_back(std::move(layer)); }

  [[nodiscard]] const LayerList& GetLayers() const { return layers_; }
  [[nodiscard]] LayerList& GetLayers() { return layers_; }
  [[nodiscard]] bool ReadXmlFile();
  [[nodiscard]] bool SaveXmlFile() const;

  void Reset();

  static std::wstring FindFile(const std::wstring& data_file);
private:
  int version_ = 1;
  std::string name_;
  std::string description_;
  std::wstring filename_;

  FileList data_files_; ///< *.mf4
  FileList db_files_; ///< *.dbc, *.a2l
  FileList hex_files_; ///< *.s19, *.hex
  FileList lab_files_; ///< *.lab
  FileList any_files_; ///< *.*

  std::vector<Layer> layers_;


  static void ReadFiles(const wxXmlNode& root, FileList& file_list);
  void ReadLayers(const wxXmlNode& root);

  void SaveConfig(wxXmlNode* root) const;
  static void SaveFiles(wxXmlNode* root, const FileList& file_list,
    const std::string& type);
  void SaveLayers(wxXmlNode* root) const;
};

}  // namespace plot


