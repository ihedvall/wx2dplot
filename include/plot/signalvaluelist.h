/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <deque>

#include "plot/signalvalue.h"

namespace plot {

class SignalValueList : public std::deque<SignalValue> {
public:
  void SetUpdated(bool updated = true) { updated_ = updated; }
  [[nodiscard]] bool IsUpdated() const { return updated_; }
private:
  bool updated_ = false;
};

} // namespace plot


