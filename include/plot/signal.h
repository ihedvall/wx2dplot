/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <string>
#include <map>
#include <mutex>
#include <algorithm>
#include <optional>

#include <plot/signalvalue.h>
#include <plot/signalvaluelist.h>

namespace plot {

using PlotEnumList = std::map<int, std::string>;

class Signal {
public:
  Signal() = default;
  virtual ~Signal() = default;

  Signal(const Signal& signal) = default;
  Signal& operator = (const Signal& signal) = default;

  bool operator < (const Signal& signal) const {
    return name_ < signal.name_;
  }

  void SetSelected(bool selected) { selected_ = selected; }
  [[nodiscard]] bool IsSelected() const { return selected_; }

  void SetShow(bool show) { show_ = show; }
  [[nodiscard]] bool IsShown() const { return show_; }

  void SetName(std::string name) { name_ = std::move(name); }
  [[nodiscard]] const std::string& GetName() const { return name_; }

  void SetDescription(std::string description) {
    description_ = std::move(description);
  }
  [[nodiscard]] const std::string& GetDescription() const { return description_; }

  void SetUnit(std::string unit) { unit_ = std::move(unit); }
  [[nodiscard]] const std::string& GetUnit() const { return unit_; }

  void SetMasterName(std::string name) { master_name_ = std::move(name); }
  [[nodiscard]] const std::string& GetMasterName() const { return master_name_; }

  void SetMasterUnit(std::string unit) { master_unit_ = std::move(unit); }
  [[nodiscard]] const std::string& GetMasterUnit() const { return master_unit_; }

  void SetMin(double min) { min_ = min; }
  [[nodiscard]] double GetMin() const;

  void SetMax(double max) { max_ = max; }
  [[nodiscard]] double GetMax() const;

  void SetHighWarning(double warning) {
    warning_ = warning;
  }
  [[nodiscard]] const std::optional<double>& GetHighWarning() const {
    return warning_;
  }
  void SetEnumList(PlotEnumList enum_list) {
    enum_list_ = std::move(enum_list);
  }
  [[nodiscard]] const PlotEnumList& GetEnumList() const {
    return enum_list_;
  }
  [[nodiscard]] std::string GetEnum(int value) const;

  void SetPrecision(int precision) { precision_ = precision; }
  [[nodiscard]] int GetPrecision() const { return precision_; }

  void SetMasterPrecision(int precision) { master_precision_ = precision; }
  [[nodiscard]] int GetMasterPrecision() const { return master_precision_; }

  [[nodiscard]] bool IsLatestUpdated() const;
  void SetLatestValue(const SignalValue& value);
  [[nodiscard]] SignalValue GetLatestValue() const;

  SignalValueList& SignalValues();
  const SignalValueList& SignalValues() const;

 private:
  bool selected_ = false; ///< Selected means that it shall be used in plot
  bool show_ = true; ///< Show means that it shall be displayed in plot

  std::string name_; ///< This will be used as a signal label.
  std::string description_;
  std::string unit_;
  PlotEnumList enum_list_;
  std::string master_name_ = "Sample"; ///< X-axis
  std::string master_unit_;;
  double min_ = 0.0; ///< Normally the min range on Y-axis
  double max_ = 0.0;
  std::optional<double> warning_ = 0.0; ///< (High) warning limit
  int precision_ = -1; ///< Number of decimals to show
  int master_precision_ = -1; ///< Number of decimals to show

  SignalValueList value_list_;

};

} // namespace plot

