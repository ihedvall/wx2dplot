/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/layer.h"

#include <string_view>
#include <array>

namespace {
constexpr std::array<std::string_view, 2>  kLayoutList = {
  "LinePlot", "Custom"
};

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
      break;;
  }
  return false;
}

}

namespace plot {

Layer::Layer(Project&& project, LayoutType layout_type)
  : Project(std::move(project)),
    layout_type_(layout_type) {
  
}

void Layer::SetLayoutTypeAsString(const std::string& type) {
  for (size_t index = 0; index < kLayoutList.size(); ++index) {
    if (type == kLayoutList[index]) {
      layout_type_ = static_cast<LayoutType>(index);
      break;
    }
  }
}

const std::string_view& Layer::GetLayoutTypeAsString() const {
  const auto index = static_cast<size_t>(layout_type_);

  return index < kLayoutList.size() ?
    kLayoutList[static_cast<size_t>(layout_type_)] :
    kLayoutList.back();
}

void Layer::ReadXml(const wxXmlNode& root) {
  std::string name = root.GetAttribute("name").utf8_string();
  std::string layout = root.GetAttribute("layout").utf8_string();

  for (wxXmlNode* child_node = root.GetChildren(); child_node != nullptr;
       child_node = child_node->GetNext()) {
    if (child_node->GetType() != wxXML_ELEMENT_NODE) {
      continue;
    }
    if (child_node->GetName() == "Name") {
      name = child_node->GetNodeContent().utf8_string();
      SetName(name);
    } else if (child_node->GetName() == "Layout") {
      layout = child_node->GetNodeContent().utf8_string();
      SetLayoutTypeAsString(layout);
    } else if (child_node->GetName() == "Description") {
      const std::string desc = child_node->GetNodeContent().utf8_string();
      SetDescription(desc);
    } else if (child_node->GetName() == "XZero") {
      const bool x_value = GetNodeBoolean(*child_node);
      SetIncludeZeroX(x_value);
    } else if (child_node->GetName() == "YZero") {
      const bool y_value = GetNodeBoolean(*child_node);
      SetIncludeZeroY(y_value);
    } else if (child_node->GetName() == "Signals") {
      ReadSignals(*child_node);
    }
  }
}

void Layer::SaveXml(wxXmlNode* root) const {
  if (root == nullptr) {
    return;
  }

  const wxString name = wxString::FromUTF8(GetName());
  const wxString type = wxString(GetLayoutTypeAsString());
  auto* layer_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Layer");
  layer_node->AddAttribute("name", name);
  layer_node->AddAttribute("layout", type);
  {
    auto* name_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Name");
    new wxXmlNode( name_node, wxXML_TEXT_NODE,
      wxEmptyString, name);
    layer_node->AddChild(name_node);
  }
  {
    auto* type_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Layout");
    new wxXmlNode( type_node, wxXML_TEXT_NODE,
      wxEmptyString, type);
    layer_node->AddChild(type_node);
  }
  {
    auto* desc_node = new wxXmlNode(wxXML_ELEMENT_NODE, "Description");
    new wxXmlNode( desc_node, wxXML_TEXT_NODE,
      wxEmptyString, wxString::FromUTF8(GetDescription()));
    layer_node->AddChild(desc_node);
  }
  {
    auto* x_node = new wxXmlNode(wxXML_ELEMENT_NODE, "XZero");
    new wxXmlNode( x_node, wxXML_TEXT_NODE,
      wxEmptyString, GetIncludeZeroX() ? "true" : "false");
    layer_node->AddChild(x_node);
  }
  {
    auto* y_node = new wxXmlNode(wxXML_ELEMENT_NODE, "YZero");
    new wxXmlNode( y_node, wxXML_TEXT_NODE,
      wxEmptyString, GetIncludeZeroY() ? "true" : "false");
    layer_node->AddChild(y_node);
  }
  {
    auto* signals = new wxXmlNode(wxXML_ELEMENT_NODE, "Signals");
    for (const auto& signal : GetSignals()) {
      signal.SaveXml(signals);
    }

    layer_node->AddChild(signals);
  }
  /*
  if (!displays_.empty()) {
    auto* displays = new wxXmlNode(wxXML_ELEMENT_NODE, "Displays");
    for (const auto& display : displays_) {
      display.SaveXml(displays);
    }

    layer_node->AddChild(displays);
  }
  */
  root->AddChild(layer_node);
}

void Layer::ReadSignals(const wxXmlNode& root) {
  ClearSignals();
  for (auto* child_node = root.GetChildren(); child_node;
       child_node = child_node->GetNext()) {
    if (child_node->GetName() == "Signal" &&
        child_node->GetType() == wxXML_ELEMENT_NODE) {
      Signal signal;
      signal.ReadXml(*child_node);
      AddSignal(signal);
    }
  }
}

} // plot