/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <set>
namespace plot {

class Project;
class Signal;

class YAxis final {
public:
  YAxis() = default;
  explicit YAxis(const Signal& signal);
  YAxis& operator=(const YAxis&) = default;

  [[nodiscard]] std::string Name() const;
  [[nodiscard]] const std::string& Unit() const { return unit_; }
  [[nodiscard]] const std::string& Description() const { return description_; }
  [[nodiscard]] double GetMin() const { return min_; }
  [[nodiscard]] double GetMax() const { return max_; }

  void AddSignal(const Signal& signal);
private:
  std::string unit_;
  std::string description_;
  double min_ = 0.0;
  double max_ = 0.0;
  std::set<const Signal*> signal_list_;
};

}  // namespace plot


