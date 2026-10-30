/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/display.h"

namespace plot {

bool Display::operator==(const Display& display) const {
  if (display_type_ != display.display_type_) {
    return false;
  }
  if (name_ != display.name_) {
    return false;
  }
  if (pos_ != display.pos_) {
    return false;
  }
  if (size_ != display.size_) {
    return false;
  }
  return true;
}

} // plot