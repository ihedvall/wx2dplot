/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
namespace plot {

class Project;
class XAxis final {
public:
  XAxis() = default;
  explicit XAxis(const Project& project);

  XAxis& operator=(const XAxis&) = default;

  [[nodiscard]] const std::string& Name() const { return name_; }
  [[nodiscard]] const std::string& Description() const { return description_; }
  [[nodiscard]] double Min() const { return min_; }
  [[nodiscard]] double Max() const { return max_; }

  [[nodiscard]] bool XAxisUnitMismatch() const { return x_axis_unit_mismatch_; }

  void Init(const Project& project);
private:
  std::string name_;
  std::string description_;
  double min_ = 0.0;
  double max_ = 0.0;
  bool x_axis_unit_mismatch_ = false;

};

}  // namespace plot

