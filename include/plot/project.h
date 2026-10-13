/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "wx/event.h"
#include "plot/signal.h"

wxDECLARE_EVENT(PROJECT_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(MASTER_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(PLOT_MASTER_CHANGED, wxCommandEvent);
namespace plot {

class Project : public wxEvtHandler {
public:
  Project() = default;
 ~Project() override = default;
  Project(const Project& project);
  Project(Project&& project) noexcept;

  Project& operator=(const Project& project);

  [[nodiscard]] bool IsEmpty() const { return signals_.empty(); }

  void SetName(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& GetName() const { return name_; }

  void SetDescription(std::string description) {
    description_ = std::move(description);
  }
  [[nodiscard]] const std::string& GetDescription() const {
    return description_;
  }

  void SetUtcBaseTime(uint64_t utc_base_time) { utc_base_time_ = utc_base_time; }
  [[nodiscard]] uint64_t GetUtcBaseTime() const { return utc_base_time_; }

  void SetIncludeZeroX(bool include_zero_x) { include_zero_x_ = include_zero_x; }
  [[nodiscard]] bool GetIncludeZeroX() const { return include_zero_x_; }

  void SetIncludeZeroY(bool include_zero_y) { include_zero_y_ = include_zero_y; }
  [[nodiscard]] bool GetIncludeZeroY() const { return include_zero_y_; }

  [[nodiscard]] const std::vector<Signal>& GetSignals() const {
    return signals_;
  }

  void AddSignal(const Signal& signal) {
    signals_.emplace_back(signal);
  }

  [[nodiscard]] long GetSignal(const std::string& name) const;
  [[nodiscard]] Signal* GetSignal(long index);
  [[nodiscard]] const Signal* GetSignal(long index) const;
  void SortSignals();
  void DeleteSignal( std::string name);
  void ClearSignals() { signals_.clear(); }

  void SetMasterReference(double value);
  [[nodiscard]] double GetMasterReference() const;

  [[nodiscard]] std::string GetMasterName() const;
  [[nodiscard]] std::string GetMasterUnit() const;
private:
  std::string name_;
  std::string description_;
  std::vector<Signal> signals_;
  uint64_t utc_base_time_ = 0;
  double master_value_ = 0.0;
  bool include_zero_x_ = true;
  bool include_zero_y_ = false;
};

} // namespace plot


