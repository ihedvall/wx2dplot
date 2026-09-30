/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/signalvaluelist.h"

namespace plot {

SignalValueList::SignalValueList(const SignalValueList& list) {
  std::lock_guard lock(value_mutex_);
  updated_ = true;
  value_list_ = list.value_list_;
  master_list_ = list.master_list_;
  min_ = list.min_;
  max_ = list.max_;
  average_ = list.average_;
}

SignalValueList& SignalValueList::operator=(const SignalValueList& list) {
  if ( this == &list ) {
    return *this;
  }

  std::lock_guard lock(value_mutex_);
  updated_ = true;
  value_list_ = list.value_list_;
  master_list_ = list.master_list_;
  min_ = list.min_;
  max_ = list.max_;
  average_ = list.average_;
  return *this;
}

void SignalValueList::SetUpdated(bool updated) {
  updated_ = updated;
}

bool SignalValueList::IsUpdated() const {
  return updated_;
}

SignalValue SignalValueList::Min() const {
  std::lock_guard lock(value_mutex_);
  CheckIfNeedCalculations();
  return min_;
}

SignalValue SignalValueList::Max() const {
  std::lock_guard lock(value_mutex_);
  CheckIfNeedCalculations();
  return max_;
}

SignalValue SignalValueList::Average() const {
  std::lock_guard lock(value_mutex_);
  CheckIfNeedCalculations();
  return average_;
}

bool SignalValueList::IsLastUpdated() const {
  std::lock_guard lock(value_mutex_);
  if (value_list_.empty()) {
    return false;
  }
  return value_list_.back().IsUpdated();
}

void SignalValueList::SetLastValue(const SignalValue& value) {
  std::lock_guard lock(value_mutex_);
  if (value_list_.empty()) {
    Append(value);
  } else {
    auto last_itr = value_list_.rbegin();
    *last_itr = value;
  }
}

SignalValue SignalValueList::GetLastValue() const {
  std::lock_guard lock(value_mutex_);
  if (value_list_.empty()) {
    return {};
  }
  return value_list_.back();
}

SignalValue SignalValueList::GetValueByMaster(double master) const {
  std::lock_guard lock(value_mutex_);
  const auto itr = master_list_.lower_bound(master);
  if (itr == master_list_.cend()) {
    return {0.0,0.0, false};
  }
  const size_t index = itr->second;
  return index < value_list_.size() ? value_list_[index] :
    SignalValue(0.0,0.0, false);
}

SignalValue SignalValueList::GetValueByIndex(size_t index) const {
  std::lock_guard lock(value_mutex_);
  return index < value_list_.size() ? value_list_[index] :
    SignalValue(0.0,0.0, false);
}

SignalValue SignalValueList::MasterMin() const {
  std::lock_guard lock(value_mutex_);
  if (master_list_.empty()) {
    return {0.0,0.0,false};
  }
  const size_t min_index = master_list_.cbegin()->second;
  return min_index < value_list_.size() ? value_list_[min_index] :
      SignalValue(0.0,0.0, false);
}

SignalValue SignalValueList::MasterMax() const {
  std::lock_guard lock(value_mutex_);
  if (master_list_.empty()) {
    return {0.0,0.0,false};
  }
  const size_t max_index = master_list_.crbegin()->second;
  return max_index < value_list_.size() ? value_list_[max_index] :
      SignalValue(0.0,0.0, false);
}

size_t SignalValueList::Size() const {
  std::lock_guard lock(value_mutex_);
  return value_list_.size();
}

bool SignalValueList::IsEmpty() const {
  std::lock_guard lock(value_mutex_);
  return value_list_.empty();
}

void SignalValueList::Clear() {
  std::lock_guard lock(value_mutex_);
  value_list_.clear();
  value_list_.shrink_to_fit();
  SetUpdated(true);
  master_list_.clear();
}

void SignalValueList::Append(const SignalValue& value) {
  std::lock_guard lock(value_mutex_);
  value_list_.emplace_back(value);
  master_list_.emplace(value.GetMasterValue(), value_list_.size() - 1);
}

void SignalValueList::Pop() {
  std::lock_guard lock(value_mutex_);
  if (!value_list_.empty()) {
    value_list_.pop_front();
  }
  master_list_.clear();
  for (size_t index = 0; index < value_list_.size(); ++index) {
    const auto& value = value_list_[index];
    const double master = value.GetMasterValue();
    master_list_.emplace(master, index);
  }
}

void SignalValueList::CalculateMinMaxAvg() const {
  if (value_list_.empty()) {
    return;
  }
  average_ = {0.0,0.0, false};
  double sum = 0.0;
  size_t count = 0;
  bool first_value = true;
  for (const SignalValue& value : value_list_) {
    const double val = value.GetValue();
    if (first_value) {
      min_ = value;
      max_ = value;
      first_value = false;
    } else {
      min_ = std::min(min_, value);
      max_ = std::max(max_, value);
    }
    if (value.IsValid()) {
      sum += val;
      ++count;
    }
  }
  if (count > 0) {
    average_ =  {sum / static_cast<double>(count),
         0.0, true};
  }
}

void SignalValueList::GetMasterList(std::set<double>& list) const {
  std::lock_guard lock(value_mutex_);
  for (const SignalValue& value : value_list_) {
    const double master = value.GetMasterValue();
    list.insert(master);
  }
}

void SignalValueList::GetValueList(std::vector<double>& y_values,
                                   std::vector<double>& x_values) const {
  try {
    y_values.reserve(value_list_.size());
    x_values.reserve(value_list_.size());
    for (const SignalValue& value : value_list_) {
      if (!value.IsValid()) {
        continue;
      }
      y_values.emplace_back(value.GetValue());
      x_values.emplace_back(value.GetMasterValue());
    }
  } catch (const std::exception&) {
    y_values.clear();
    x_values.clear();
  }
}

void SignalValueList::CheckIfNeedCalculations() const {
  const size_t current_size = value_list_.size();
  if (current_size != current_size_) {
    current_size_ = current_size;
    CalculateMinMaxAvg();
  }
}
} // plot