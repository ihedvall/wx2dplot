/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/configuration.h"

#include <filesystem>

#include <wx/xml/xml.h>


#include "plot/plotlogstream.h"

using namespace std::filesystem;

namespace {
constexpr int kCurrentVersion = 1;

}
namespace plot {

Configuration::Configuration() {
  version_ = kCurrentVersion;
}

bool Configuration::AddDataFile(const std::wstring& data_file) {
  try {
    std::wstring filename = FindFile(data_file);
    const path fullname(data_file);
    const path stem = fullname.stem();
    if (exists(fullname)) {
      data_files_.emplace( stem.string(), data_file );
    } else {
      throw std::runtime_error("File does not exist: " + fullname.string());
    }
  } catch (const std::exception& err) {
    PLOT_ERROR() << "File path error. Error: " << err.what();
    return false;
  }
  return true;
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
  try {
    const path fullname(data_file);
    if (exists(fullname)) {
      return fullname.wstring();
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
    PLOT_ERROR() << "Failed to find the file. Error: " << err.what();
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
      std::string name = child_node->GetAttribute("name",
        wxEmptyString).utf8_string();
      const std::wstring filename = child_node->GetNodeContent().ToStdWstring();
      if (name.empty()) {
        try {
          path fullname(filename);
          name = fullname.stem().string();
        } catch (const std::exception& err) {
          PLOT_ERROR() << "Failed to read file name. Error: " << err.what();
          continue;
        }
      }
      file_list.emplace(name, filename);
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

  for (const auto& [name, file_path] : file_list) {
    auto* file = new wxXmlNode(wxXML_ELEMENT_NODE, "File");
    file->AddAttribute("name", wxString::FromUTF8(name));
    new wxXmlNode(file, wxXML_TEXT_NODE, wxEmptyString,
      wxString(file_path));
    files->AddChild(file);
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