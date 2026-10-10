/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <vector>

#include "plot/project.h"
#include "plot/display.h"

namespace plot {

enum class LayoutType {
  kLinePlotLayout,
  kCustomLayout,
};

class Layer : public Project {
public:
  Layer() = default;
  Layer(Project&& project, LayoutType layout_type);

  void SetLayoutType(LayoutType type) { layout_type_ = type; }
  [[nodiscard]] LayoutType GetLayoutType() const { return layout_type_; }

private:
  LayoutType layout_type_ = LayoutType::kCustomLayout;
  std::vector<Display> displays_;
};

}  // namespace plot


