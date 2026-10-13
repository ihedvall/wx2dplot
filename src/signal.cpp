/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/signal.h"

#include <utility>

namespace {
  bool GetNodeBoolean(const wxXmlNode& node) {
    const std::string value = node.GetNodeContent().utf8_string();
    if (value.empty()) {
      return false;
    }
    switch (value[0]) {
      case 't':
      case 'T':
      case '1':
      case 'y':
      case 'Y':
        return true;

      default:
        break;
    }
    return false;
  }

}

namespace plot {
double Signal::GetMin() const {
  if (min_ < max_) {
    return min_;
  }
  const SignalValue min = value_list_.Min();
  return min.IsValid() ? min.GetValue() : 0.0;
}

double Signal::GetMax() const {
  if (min_ < max_) {
    return max_;
  }
  const SignalValue max = value_list_.Max();
  return max.IsValid() ? max.GetValue() : 0.0;
}

std::string Signal::GetEnum(int value) const {
  if (const auto& itr = enum_list_.find(value);
      itr != enum_list_.cend()) {
    return itr->second;
      }
  return {};
}

bool Signal::IsLatestUpdated() const {
  return value_list_.IsLastUpdated();
}

void Signal::SetLatestValue(const SignalValue& value) {
  value_list_.SetLastValue(value);
}

SignalValue Signal::GetLatestValue() const {
  return value_list_.GetLastValue();
}
SignalValueList& Signal::SignalValues() {
  return value_list_;
}
const SignalValueList& Signal::SignalValues() const {
  return value_list_;
}

void Signal::ReadXml(const wxXmlNode& root) {
  name_ = root.GetAttribute("name").utf8_string();
  source_ = root.GetAttribute("source").utf8_string();
  for (auto* child_node = root.GetChildren(); child_node;
       child_node = child_node->GetNext()) {
    if (child_node->GetType() != wxXML_ELEMENT_NODE) {
      continue;
    }

    if (child_node->GetName() == "Name") {
      name_ = child_node->GetNodeContent().utf8_string();
    } else if (child_node->GetName() == "Source") {
      source_ = child_node->GetNodeContent().utf8_string();
    } else if (child_node->GetName() == "Description") {
      description_ = child_node->GetNodeContent().utf8_string();
    } else if (child_node->GetName() == "Unit") {
      unit_ = child_node->GetNodeContent().utf8_string();
    } else if (child_node->GetName() == "MasterName") {
      master_name_ = child_node->GetNodeContent().utf8_string();
    } else if (child_node->GetName() == "MasterUnit") {
      master_unit_ = child_node->GetNodeContent().utf8_string();
    } else if (child_node->GetName() == "Min") {
      child_node->GetNodeContent().ToDouble(&min_);
    } else if (child_node->GetName() == "Max") {
      child_node->GetNodeContent().ToCDouble(&max_);
    } else if (child_node->GetName() == "Selected") {
      selected_ = GetNodeBoolean(*child_node);
    } else if (child_node->GetName() == "Active") {
      active_ = GetNodeBoolean(*child_node);
    }
  }
}

void Signal::SaveXml(wxXmlNode* root) const {
  if (root == nullptr || name_.empty()) {
    return;
  }

  const wxString name = wxString::FromUTF8(name_);
  const wxString source = wxString::FromUTF8(source_);
  auto* signal = new wxXmlNode(wxXML_ELEMENT_NODE, "Signal");
  signal->AddAttribute("name", name);
  signal->AddAttribute("source", source);
  {
    auto* selected_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Selected");
    new wxXmlNode(selected_node, wxXML_TEXT_NODE, wxEmptyString,
      IsSelected() ? "true" : "false");
    signal->AddChild(selected_node);
  }
  {
    auto* active_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Active");
    new wxXmlNode(active_node, wxXML_TEXT_NODE, wxEmptyString,
      IsActive() ? "true" : "false");
    signal->AddChild(active_node);
  }
  {
    auto* name_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Name");
    new wxXmlNode(name_node, wxXML_TEXT_NODE, wxEmptyString,
      name);
    signal->AddChild(name_node);
  }
  if (!description_.empty() ){
    auto* desc_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Description");
    new wxXmlNode(desc_node, wxXML_TEXT_NODE, wxEmptyString,
      wxString::FromUTF8(description_));
    signal->AddChild(desc_node);
  }
  if (!unit_.empty() ) {
    auto* unit_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Unit");
    new wxXmlNode(unit_node, wxXML_TEXT_NODE, wxEmptyString,
      wxString::FromUTF8(unit_));
    signal->AddChild(unit_node);
  }
  {
    auto* source_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Source");
    new wxXmlNode(source_node, wxXML_TEXT_NODE, wxEmptyString,
      source);
    signal->AddChild(source_node);
  }
  if (!master_name_.empty()) {
    auto* name_node = new wxXmlNode(wxXML_ELEMENT_NODE, "MasterName");
    new wxXmlNode(name_node, wxXML_TEXT_NODE, wxEmptyString,
      master_name_);
    signal->AddChild(name_node);
  }
  if (!master_unit_.empty()) {
    auto* unit_node = new wxXmlNode(wxXML_ELEMENT_NODE, "MasterUnit");
    new wxXmlNode(unit_node, wxXML_TEXT_NODE, wxEmptyString,
      wxString::FromUTF8(unit_));
    signal->AddChild(unit_node);
  }
  if (min_ < max_) {
    {
      auto* min_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Min");
      new wxXmlNode(min_node, wxXML_TEXT_NODE, wxEmptyString,
        wxString::FromCDouble(min_));
      signal->AddChild(min_node);
    }
    {
      auto* max_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Max");
      new wxXmlNode(max_node, wxXML_TEXT_NODE, wxEmptyString,
        wxString::FromCDouble(max_));
      signal->AddChild(max_node);
    }
  }
  root->AddChild(signal);
}

} // plot