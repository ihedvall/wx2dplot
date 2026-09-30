/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <deque>
#include <map>
#include <set>
#include <mutex>
#include <atomic>

#include "plot/signalvalue.h"
#include "plot/plotlogstream.h"

namespace plot {

using MasterValueList = std::map<double, size_t>;
using PlotValueList = std::deque<SignalValue>;

class SignalValueList {
public:
  SignalValueList() = default;
  SignalValueList(const SignalValueList& list);
  SignalValueList& operator = (const SignalValueList& list);

  void SetUpdated(bool updated = true);
  [[nodiscard]] bool IsUpdated() const;

  SignalValue Min() const;
  SignalValue Max() const;
  SignalValue Average() const;

  SignalValue MasterMin() const;
  SignalValue MasterMax() const;
  [[nodiscard]] size_t Size() const;
  [[nodiscard]] bool IsEmpty() const;

  [[nodiscard]] bool IsLastUpdated() const;
  void SetLastValue(const SignalValue& value);
  [[nodiscard]] SignalValue GetLastValue() const;

  [[nodiscard]] SignalValue GetValueByMaster(double master) const;
  [[nodiscard]] SignalValue GetValueByIndex(size_t index) const;

  void Clear();
  void Append(const SignalValue& value);
  void Pop();

  template <typename T>
  void AttachValues(const T& values);

  void GetMasterList(std::set<double>& list) const;
  void GetValueList(std::vector<double>& y_values,
                    std::vector<double>& x_values) const;
private:
  mutable std::mutex value_mutex_;
  std::atomic<bool> updated_ = false;
  PlotValueList value_list_;
  MasterValueList master_list_;
  mutable SignalValue min_;
  mutable SignalValue max_;
  mutable SignalValue average_;

  mutable size_t current_size_ = 0;

  void CalculateMinMaxAvg() const;
  void CheckIfNeedCalculations() const;
};

template <typename T>
void SignalValueList::AttachValues(const T &values) {
  std::lock_guard lock(value_mutex_);
  try {
    value_list_.clear();
    value_list_.shrink_to_fit();
    value_list_.resize(values.size());
    std::copy(values.cbegin(), values.cend(), value_list_.begin());
    SetUpdated(true);

    master_list_.clear();
    for (size_t index = 0; index < value_list_.size(); ++index) {
      const auto& value = value_list_[index];
      const double master = value.GetMasterValue();
      master_list_.emplace(master, index);
    }
    CalculateMinMaxAvg();
  } catch (const std::exception& err) {
    PLOT_ERROR() <<"Failed to copy values. Error: " << err.what();
    value_list_.clear();
    value_list_.shrink_to_fit();
    SetUpdated(true);
    master_list_.clear();
  }
}

} // namespace plot


