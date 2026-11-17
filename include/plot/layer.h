/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <wx/xml/xml.h>

#include "plot/project.h"
#include "plot/display.h"

namespace plot {

enum class LayoutType {
  kLinePlotLayout = 0,
  kCustomLayout,
};

class Layer : public Project {
public:
  Layer() = default;
  Layer(Project&& project, LayoutType layout_type);

  bool operator==(const Layer& layer) const;

  void SetIndex(size_t index) { index_ = index; }
  [[nodiscard]] size_t GetIndex() const { return index_; }

  void SetLayoutType(LayoutType type) { layout_type_ = type; }
  [[nodiscard]] LayoutType GetLayoutType() const { return layout_type_; }
  void SetLayoutTypeAsString(const std::string& type);
  [[nodiscard]] const std::string_view& GetLayoutTypeAsString() const;

  void ReadXml(const wxXmlNode& root);
  void SaveXml(wxXmlNode* root) const;

private:
  LayoutType layout_type_ = LayoutType::kCustomLayout;
  size_t index_ = 0; ///< User defined index
  std::vector<Display> displays_;

  void ReadSignals(const wxXmlNode& root);
};

}  // namespace plot


