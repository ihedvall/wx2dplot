/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/configuration.h"

#include <filesystem>
#include <array>
#include <functional>

#include <boost/algorithm/string.hpp>

#include <wx/xml/xml.h>

#include "plot/plotlogstream.h"

using namespace std::filesystem;
using namespace boost::algorithm;

namespace {
constexpr int kCurrentVersion = 1;

}
namespace plot {

Configuration::Configuration() {
  version_ = kCurrentVersion;
}

Configuration::Configuration(const Configuration& config)
  : version_(kCurrentVersion),
    name_(config.name_),
    description_(config.description_),
    filename_(config.filename_),
    data_files_(config.data_files_),
    db_files_(config.db_files_),
    hex_files_(config.hex_files_),
    lab_files_(config.lab_files_),
    any_files_(config.any_files_),
    layers_(config.layers_)
{
}

Configuration& Configuration::operator=(const Configuration& config) {
  if (this == &config) {
    return *this;
  }
  version_ = kCurrentVersion;
  name_ =config.name_;
  description_ = config.description_;
  filename_ = config.filename_;
  data_files_ = config.data_files_;
  db_files_ = config.db_files_;
  hex_files_ = config.hex_files_;
  lab_files_ = config.lab_files_;
  any_files_ = config.any_files_;
  layers_ = config.layers_;
  return *this;
}

bool Configuration::operator==(const Configuration& config) const {
  if (name_ != config.name_) {
    return false;
  }
  if (description_ != config.description_) {
    return false;
  }
  if (filename_ != config.filename_) {
    return false;
  }
  if (data_files_ != config.data_files_) {
    return false;
  }
  if (db_files_ != config.db_files_) {
    return false;
  }
  if (hex_files_ != config.hex_files_) {
    return false;
  }
  if (lab_files_ != config.lab_files_) {
    return false;
  }
  if (any_files_ != config.any_files_) {
    return false;
  }
  if (layers_ != config.layers_) {
    return false;
  }
  return true;
}

std::wstring_view Configuration::GetFileWildcardFilter() {
  constexpr std::wstring_view filter =
L"MDF Data Files (*.mdf;*.mf4;*.mf3;*.dat)|*.mdf;*.mf4;*.mf3;*.dat"
"|Database Files (*.dbc;*.dbf;*.a2l;*.flx;*.ldf)|*.dbc;*.dbf;*.a2l;*.flx;*.ldf"
"|Selection List Files (*.lab)|*lab"
"|Binary Files (*.hex;*.bin;*.s19;*.s28;*.s37)|*.hex;*.bin;*.s19;*.s28;*.s37"
"|All Files (*.*)|*.*";
  return filter;
}

bool Configuration::AddFile(const std::wstring& file) {
  try {
    std::wstring filename = FindFile(file);
    const path fullname(filename);
    const std::wstring extension = fullname.extension().wstring();
    if (!exists(fullname)) {
      throw std::runtime_error("File does not exist");
    }
    if (is_regular_file(fullname)) {
      throw std::runtime_error("File is not a regular file");
    }
    const bool mdf = iequals(extension, L".mdf")
                     || iequals(extension, L".mf4")
                     || iequals(extension, L".mf3")
                     || iequals(extension, L".dat");
    const bool database = iequals(extension, L".dbc")
                          || iequals(extension, L".a2l")
                          || iequals(extension, L".flx")
                          || iequals(extension, L".ldf")
                          || iequals(extension, L".sqlite");
    const bool lab = iequals(extension, L".lab");
    const bool hex = iequals(extension, L".hex")
                     || iequals(extension, L".bin")
                     || iequals(extension, L".s19")
                     || iequals(extension, L".s28")
                     || iequals(extension, L".s37");
    if (mdf) {
      data_files_.emplace_back(std::move(filename));
    } else if (database) {
      db_files_.emplace_back(std::move(filename));
    } else if (lab) {
      lab_files_.emplace_back(std::move(filename));
    } else if (hex) {
      hex_files_.emplace_back(std::move(filename));
    } else {
      any_files_.emplace_back(std::move(filename));
    }
  } catch (const std::exception& err) {
    const wxString fname(filename_);
    PLOT_ERROR() << "File path error. Error: " << err.what()
      << ", File: " << fname;
    return false;
  }
  return true;
}

void Configuration::DelFile(const std::wstring& file) {
  std::array<std::reference_wrapper<FileList>, 5> file_list = {
    data_files_, db_files_, lab_files_, hex_files_, any_files_
  };
  try {
    // First try is to use the full path
    const path fullname(file);
    for (auto& list : file_list ) {
      FileList& flist = list.get();
      for (auto itr = flist.begin();itr != flist.end(); ++itr) {
        const path fname(*itr);
        if (fullname == fname) {
          flist.erase(itr);
          return;
        }
      }
    }

    // Next try with the filename
    const std::wstring filename = fullname.filename().wstring();
    for (auto& list : file_list ) {
      FileList& flist = list.get();
      for (auto itr = flist.begin();itr != flist.end(); ++itr) {
        const path fname(*itr);
        if (iequals(filename,fname.filename().wstring()) ) {
          flist.erase(itr);
          return;
        }
      }
    }
  } catch (const std::exception& err) {
    const wxString fname(file);
    PLOT_ERROR() << "File deletion error. Error: " << err.what()
      << ", File: " << fname;
    return;
  }
  {
    const wxString fname(file);
    PLOT_ERROR() << "File deletion error. Error: File not found"
      << ", File: " << fname;
  }
}

bool Configuration::ReadXmlFile() {
  try {
    const wxString file(filename_);
    const path fullname(filename_);
    if (!exists(fullname)) {
      throw std::runtime_error("File does not exist: " + file);
    }

    wxXmlDocument xml_doc;
    const bool load = xml_doc.Load(file);
    if (!load) {
      throw std::runtime_error("Error loading XML file");
    }
    const wxXmlNode* root_node = xml_doc.GetRoot();
    if (root_node == nullptr) {
      throw std::runtime_error("No XML root node");
    }
    if (root_node->GetName() != "Configuration") {
      throw std::runtime_error("No Configuration root node");
    }

    for (const wxXmlNode* child_node = root_node->GetChildren();
         child_node != nullptr;
         child_node = child_node->GetNext()) {
      if (child_node->GetType() != wxXML_ELEMENT_NODE) {
        continue;
      }
      if (child_node->GetName() == "Name") {
        name_ = child_node->GetNodeContent().utf8_string();
      } else if (child_node->GetName() == "Description") {
        description_ = child_node->GetNodeContent().utf8_string();
      } else if (child_node->GetName() == "Version") {
        child_node->GetNodeContent().ToInt(&version_);
      } else if (child_node->GetName() == "DataFiles") {
        ReadFiles(*child_node, data_files_);
      } else if (child_node->GetName() == "DbFiles") {
        ReadFiles(*child_node, db_files_);
      } else if (child_node->GetName() == "HexFiles") {
        ReadFiles(*child_node, hex_files_);
      } else if (child_node->GetName() == "LabFiles") {
        ReadFiles(*child_node, lab_files_);
      } else if (child_node->GetName() == "AnyFiles") {
        ReadFiles(*child_node, any_files_);
      } else if (child_node->GetName() == "Layers") {
        ReadLayers(*child_node);
      }
    }
  } catch (const std::exception& err) {
    PLOT_ERROR() << "Error saving XML file. Error: " << err.what();
    return false;
  }
  return true;
}

bool Configuration::SaveXmlFile() const {
  try {
    wxXmlDocument xml_doc;
    xml_doc.SetFileEncoding("UTF-8");
    auto* root_node = new wxXmlNode(wxXML_ELEMENT_NODE,
      "Configuration");
    SaveConfig(root_node);
    SaveFiles(root_node, data_files_, "DataFiles");
    SaveFiles(root_node, db_files_, "DbFiles");
    SaveFiles(root_node, hex_files_, "HexFiles");
    SaveFiles(root_node, lab_files_, "LabFiles");
    SaveFiles(root_node, any_files_, "AnyFiles");
    SaveLayers(root_node);
    xml_doc.SetRoot(root_node);


    if (filename_.empty()) {
      throw std::runtime_error("No filename specified");
    }
    const path fullname(filename_);
    if (fullname.has_parent_path()) {
      const path parent_path = fullname.parent_path();
      create_directories(parent_path);
    }
    const bool save = xml_doc.Save(wxString(filename_));
    if (!save) {
      throw std::runtime_error("Error saving XML file");
    }
  } catch (const std::exception& err) {
    PLOT_ERROR() << "Error saving XML file. Error: " << err.what();
    return false;
  }

  return true;
}

void Configuration::Reset() {
  name_.clear();
  description_.clear();
  filename_.clear();
  data_files_.clear();
  db_files_.clear();
  hex_files_.clear();
  lab_files_.clear();
  any_files_.clear();
}

std::wstring Configuration::FindFile(const std::wstring& data_file) {
  const wxString fname(data_file);
  try {
    const path fullname(data_file);
    if (exists(fullname)) {
      return data_file;
    }
    const path file_name = fullname.filename();
    // Search in sub-directories
    if (fullname.has_parent_path() && fullname.has_filename()) {
      const path parent_path = fullname.parent_path();
      for (const directory_entry& dir_entry :
           recursive_directory_iterator(parent_path)) {
        if (!dir_entry.is_regular_file()) {
          continue;
        }
        const path temp_name = dir_entry.path().filename();
        const int comp = file_name.compare(temp_name);
        if (comp == 0) {
          return dir_entry.path().wstring();
        }
      }
    }

    // Search in current path
    if (fullname.has_filename()) {
      const path current = current_path();
      for (const directory_entry& dir_entry :
           recursive_directory_iterator(current)) {
        if (!dir_entry.is_regular_file()) {
          continue;
        }
        const path temp_name = dir_entry.path().filename();
        const int comp = file_name.compare(temp_name);
        if (comp == 0) {
          return dir_entry.path().wstring();
        }
      }
    }
  }
  catch (const std::exception& err) {
    PLOT_ERROR() << "Failed to find the file. Error: " << err.what()
      << ", File: " << fname;
  }
  return data_file;
}

void Configuration::ReadFiles(const wxXmlNode& root, FileList& file_list) {
  file_list.clear();
  for (const wxXmlNode* child_node = root.GetChildren();
       child_node != nullptr; child_node = child_node->GetNext()) {
    if (child_node->GetType() != wxXML_ELEMENT_NODE) {
      continue;
    }
    if (child_node->GetName() == "File") {
      std::wstring filename = child_node->GetNodeContent().ToStdWstring();
      file_list.emplace_back(std::move(filename));
    }
  }
}

void Configuration::ReadLayers(const wxXmlNode& root) {
  layers_.clear();
  for (wxXmlNode* child_node = root.GetChildren(); child_node != nullptr;
       child_node = child_node->GetNext()) {
    if (child_node->GetType() != wxXML_ELEMENT_NODE) {
      continue;
    }
    if (child_node->GetName() == "Layer") {
      Layer layer;
      layer.ReadXml(*child_node);
      layers_.emplace_back(std::move(layer));
    }
  }
}

void Configuration::SaveConfig(wxXmlNode* root) const {
  {
    auto* version = new wxXmlNode(wxXML_ELEMENT_NODE,  "Version");
    new wxXmlNode(version,wxXML_TEXT_NODE, wxEmptyString,
      wxString::FromUTF8(std::to_string(kCurrentVersion)));
    root->AddChild(version);
  }
  {
    auto* name = new wxXmlNode(wxXML_ELEMENT_NODE,  "Name");
    new wxXmlNode(name,wxXML_TEXT_NODE, wxEmptyString,
      wxString::FromUTF8(GetName()));
    root->AddChild(name);
  }
  {
    auto* desc = new wxXmlNode(wxXML_ELEMENT_NODE,  "Description");
    new wxXmlNode(desc, wxXML_TEXT_NODE, wxEmptyString,
                  wxString::FromUTF8(description_));
    root->AddChild(desc);
  }
}

void Configuration::SaveFiles(wxXmlNode* root, const FileList& file_list,
  const std::string& type) {
  auto* files = new wxXmlNode(wxXML_ELEMENT_NODE, type);

  for (const auto& file_path : file_list) {
    try {
      const path fullname(file_path);
      const std::wstring name = fullname.stem().wstring();
      auto* file = new wxXmlNode(wxXML_ELEMENT_NODE, "File");
      file->AddAttribute("name", wxString(name));
      new wxXmlNode(file, wxXML_TEXT_NODE, wxEmptyString,
        wxString(file_path));
      files->AddChild(file);
    }
    catch (const std::exception& error) {
      PLOT_ERROR() << "Failed to save file. Error: " <<  error.what();
    }
  }
  root->AddChild(files);
}

void Configuration::SaveLayers(wxXmlNode* root) const {
  auto* layers = new wxXmlNode(wxXML_ELEMENT_NODE, "Layers");

  for (const auto& layer : layers_) {
    layer.SaveXml(layers);

  }
  root->AddChild(layers);
}

} // plot