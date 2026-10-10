/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <wx/position.h>
#include <wx/gdicmn.h>

namespace plot {

enum class DisplayType {
  kLinePlotDisplay = 0,
  kLegendDisplay,
  kAlphanumericDisplay,
  kProjectDisplay,
  kCustomDisplay,
};

class Display {
public:
  void SetDisplayType(DisplayType type) { display_type_ = type; }
  [[nodiscard]] DisplayType GetDisplayType() const { return display_type_; }

  void SetName(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& GetName() const { return name_; }

  void SetPosition(wxPosition pos) { pos_ = pos; }
  [[nodiscard]] const wxPosition& GetPosition() const { return pos_; }

  void SetSize(wxSize size) { size_ = size; }
  [[nodiscard]] const wxSize& GetSize() const { return size_; }

private:
  DisplayType display_type_ = DisplayType::kCustomDisplay;
  std::string name_;
  wxPosition pos_;
  wxSize size_;
};

}  // namespace plot


