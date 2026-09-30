/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <deque>

#include "plot/yaxis.h"

namespace plot {

class Signal;
class YAxisList final {
public:
  YAxisList() = default;
  explicit YAxisList(const Project& project);

  void Init(const Project& project);

  [[nodiscard]] size_t NofAxis() const { return y_axis_list_.size(); }
  [[nodiscard]] const YAxis* GetAxis(size_t index) const;

  [[nodiscard]] size_t NofSignals() const { return signal_list_.size(); }
  [[nodiscard]] const Signal* GetSignal(size_t index) const;

  [[nodiscard]] bool IsEmpty() const { return y_axis_list_.empty(); }
  [[nodiscard]] YAxis* FindYAxis(const Signal& signal) const;
private:
  std::deque<YAxis> y_axis_list_;
  std::deque<const Signal*> signal_list_;



};

}  // namespace plot


