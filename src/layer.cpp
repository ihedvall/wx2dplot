/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/layer.h"

namespace plot {

Layer::Layer(Project&& project, LayoutType layout_type)
  : Project(std::move(project)),
    layout_type_(layout_type) {
  
}
} // plot