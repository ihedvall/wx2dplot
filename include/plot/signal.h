/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <string>
#include <map>
#include <mutex>
#include <vector>

#include <plot/signalvalue.h>
#include <plot/signalvaluelist.h>

namespace plot {

using PlotEnumList = std::map<int, std::string>;

class Signal {
public:
  Signal() = default;
  virtual ~Signal() = default;

  Signal(const Signal& signal);
  Signal& operator = (const Signal& signal);

  bool operator < (const Signal& signal) const {
    return name_ < signal.name_;
  }

  void SetSelected(bool selected) { selected_ = selected; }
  [[nodiscard]] bool IsSelected() const { return selected_; }

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
  [[nodiscard]] double GetMin() const { return min_; }

  void SetMax(double max) { max_ = max; }
  [[nodiscard]] double GetMax() const { return max_; }

  void SetEnumList(PlotEnumList enum_list) {
    enum_list_ = std::move(enum_list);
  }
  [[nodiscard]] const PlotEnumList& GetEnumList() const {
    return enum_list_;
  }
  [[nodiscard]] std::string GetEnum(int value) const;

  [[nodiscard]] bool IsLatestUpdated() const;
  void SetLatestValue(double value, double master_value, bool valid = true);
  [[nodiscard]] bool GetLatestValue(double& value, double& master_value) const;


  void AttachValues(const std::vector<SignalValue>& values);
  void ResetValues();
  void AddValue(const SignalValue& value);
  void PopValue();
  void CalculateMinMax(bool include_zero);

  [[nodiscard]]SignalValue GetValueByIndex(size_t index) const;
  [[nodiscard]] double GetMasterMin() const;
  [[nodiscard]] double GetMasterMax() const;
  [[nodiscard]] size_t GetSize() const;
private:
  bool selected_ = false;
  std::string name_; ///< This will be used as a signal label.
  std::string description_;
  std::string unit_;
  PlotEnumList enum_list_;
  std::string master_name_ = "Sample"; ///< X-axis
  std::string master_unit_;;
  double min_ = 0.0; ///< Normally the min range on Y-axis
  double max_ = 0.0;

  mutable std::mutex value_mutex_;
  SignalValue lrv_;
  SignalValueList value_list_;


};

} // namespace plot

