/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/configuration.h"

#include <filesystem>

#include <wx/xml/xml.h>

#include "plot/plotlogstream.h"

using namespace std::filesystem;
namespace plot {

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
    for (const wxXmlNode* child_node = root_node->GetChildren();
         child_node != nullptr;
         child_node = child_node->GetNext()) {
      if (child_node->GetName() == "Name") {
        name_ = child_node->GetContent().ToUTF8();
      } else if (child_node->GetName() == "Description") {
        description_ = child_node->GetContent().ToUTF8();
      }
    }
  } catch (const std::exception& err) {
    PLOT_ERROR() << "Error saving XML file. Error: " << err.what();
    return false;
  }

  return true;
}

bool Configuration::SaveXmlFile() {
  try {
    wxXmlDocument xml_doc;
    xml_doc.SetFileEncoding("UTF-8");
    auto* root_node = new wxXmlNode(wxXML_ELEMENT_NODE,
      "Configuration");
    xml_doc.SetRoot(root_node);
    new wxXmlNode(root_node, wxXML_TEXT_NODE, "Name",
                  wxString::FromUTF8(name_));
    new wxXmlNode(root_node, wxXML_TEXT_NODE, "Description",
                  wxString::FromUTF8(description_));

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

} // plot