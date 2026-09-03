/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/signal.h"

#include <utility>

namespace plot {

Signal::Signal(const Signal &signal)
: selected_(signal.selected_),
  name_(signal.name_),
  description_(signal.description_),
  unit_(signal.unit_),
  enum_list_(signal.enum_list_),
  master_name_(signal.master_name_),
  master_unit_(signal.master_unit_),
  min_(signal.min_),
  max_(signal.max_),
  lrv_(signal.lrv_),
  value_list_(signal.value_list_)
{
}

Signal &Signal::operator=(const Signal &signal) {
  if (&signal == this) {
    return  *this;
  }
  selected_ = signal.selected_;
  name_ = signal.name_;
  description_ = signal.description_;
  unit_ = signal.unit_;
  enum_list_ = signal.enum_list_;
  master_name_ = signal.master_name_;
  master_unit_ = signal.master_unit_;
  min_ = signal.min_;
  max_ = signal.max_;
  lrv_ = signal.lrv_;
  value_list_ = signal.value_list_;
  return *this;
}

std::string Signal::GetEnum(int value) const {
  if (const auto& itr = enum_list_.find(value);
      itr != enum_list_.cend()) {
    return itr->second;
  }
  return {};
}

bool Signal::IsLatestUpdated() const {
  std::lock_guard lock(value_mutex_);
  return lrv_.IsUpdated();
}

void Signal::SetLatestValue(double value, double master_value, bool valid) {
  std::lock_guard lock(value_mutex_);
  return lrv_.SetValue(value, master_value, valid);
}

bool Signal::GetLatestValue(double &value, double &master_value) const {
  std::lock_guard lock(value_mutex_);
  return lrv_.GetValue(value, master_value);
}

void Signal::AttachValues(const std::vector<SignalValue> &values) {
  std::lock_guard lock(value_mutex_);
  value_list_.clear();
  value_list_.insert(value_list_.end(),
                     values.cbegin(), values.cend());
  // Update last reported value
  if (!value_list_.empty()) {
    const SignalValue& last_value = value_list_.back();
    lrv_ = last_value;
  }
}

void Signal::ResetValues() {
  std::lock_guard lock(value_mutex_);
  value_list_.clear();
  value_list_.shrink_to_fit();
  value_list_.SetUpdated(true);
  lrv_.SetValue(0.0,0.0,false);
}

void Signal::AddValue(const SignalValue& value) {
  std::lock_guard lock(value_mutex_);
  value_list_.emplace_back(value);
  lrv_ = value;
}

void Signal::PopValue() {
  std::lock_guard lock(value_mutex_);
  value_list_.pop_front();
  if (!value_list_.empty()) {
    lrv_.SetValue(0.0, 0.0, false);;
  }
}

void Signal::CalculateMinMax(bool include_zero) {
  std::lock_guard lock(value_mutex_);

  if (value_list_.empty()) {
    return;
  }
  bool first_value = true;
  for (const SignalValue& value : value_list_) {
    const double val = value.GetValue();
    if (first_value) {
      min_ = val;
      max_ = val;
      first_value = false;
    } else {
      min_ = std::min(min_, val);
      max_ = std::max(max_, val);
    }
  }

  if (include_zero) {
    if (max_ < 0.0 && min_ < 0.0) {
      max_ = 0.0;
    } else if (max_ > 0.0 && min_ > 0.0) {
      min_ = 0.0;
    }
  }
}

SignalValue Signal::GetValueByIndex(size_t index) const {
  SignalValue value;
  std::lock_guard lock(value_mutex_);
  if (index < value_list_.size()) {
    value = value_list_[index];
  }
  return value;
}

double Signal::GetMasterMin() const {
  std::lock_guard lock(value_mutex_);
  return value_list_.empty() ? 0.0 : value_list_.front().GetValue();
}

double Signal::GetMasterMax() const {
  std::lock_guard lock(value_mutex_);
  return value_list_.empty() ? 0.0 : value_list_.back().GetValue();
}

size_t Signal::GetSize() const {
  std::lock_guard lock(value_mutex_);
  return value_list_.size();
}

} // plot