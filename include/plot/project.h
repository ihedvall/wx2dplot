/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>
#include "plot/signal.h"

namespace plot {

class Project {
public:
  void SetName(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& GetName() const { return name_; }

  void SetDescription(std::string description) {
    description_ = std::move(description);
  }
  [[nodiscard]] const std::string& GetDescription() const {
    return description_;
  }

  void SetFilename(std::wstring filename) { filename_ = std::move(filename); }
  [[nodiscard]] const std::wstring& GetFilename() const { return filename_; }

  [[nodiscard]] const std::vector<Signal>& GetSignals() const {
    return signals_;
  }

  void AddSignal(const Signal& signal) {
    signals_.emplace_back(signal);
  }

  [[nodiscard]] long GetSignal(const std::string& name) const;
  [[nodiscard]] Signal* GetSignal(long index);
  void SortSignals();
  void DeleteSignal( std::string name);

private:
  std::string name_;
  std::string description_;
  std::wstring filename_;
  std::vector<Signal> signals_;
};

} // namespace plot


