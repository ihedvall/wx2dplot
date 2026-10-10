/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "plot/project.h"

#include <algorithm>

wxDEFINE_EVENT(PROJECT_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(MASTER_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(PLOT_MASTER_CHANGED, wxCommandEvent);


namespace {

struct
{
  bool operator()(const plot::Signal& a, const plot::Signal& b) const {
    return a < b;
  }
} SignalLess;

}

namespace plot {

Project::Project(const Project& project)
  : wxEvtHandler(),
    name_(project.name_),
    description_(project.description_),
    signals_(project.signals_),
    utc_base_time_(project.utc_base_time_),
    master_value_(0.0),
    include_zero_x_(project.include_zero_x_),
    include_zero_y_(project.include_zero_y_) {
}


Project::Project(Project&& project)
 noexcept   : wxEvtHandler(),
    name_( std::move(project.name_)),
    description_( std::move(project.description_)),
    signals_(std::move(project.signals_)),
    utc_base_time_(project.utc_base_time_),
    master_value_(0.0),
    include_zero_x_(project.include_zero_x_),
    include_zero_y_(project.include_zero_y_) {
}

Project& Project::operator = (const Project& project) {
  if (this == &project) {
    return *this;
  }
  name_ = project.name_;
  description_ = project.description_;
  signals_ = project.signals_;
  utc_base_time_ = project.utc_base_time_;
  master_value_ = 0.0,
  include_zero_x_ = project.include_zero_x_;
  include_zero_y_ = project.include_zero_y_;
  return *this;
}


long Project::GetSignal(const std::string &name) const {
  for (size_t index = 0; index < signals_.size(); ++index) {
    const Signal& signal = signals_[index];
    if (signal.GetName() == name) {
      return static_cast<long>(index);
    }
  }
  return -1;
}

Signal* Project::GetSignal(long index) {
  if (index < 0 || index >= static_cast<long>(signals_.size())) {
    return nullptr;
  }
  return &signals_[index];
}

const Signal* Project::GetSignal(long index) const {
  if (index < 0 || index >= static_cast<long>(signals_.size())) {
    return nullptr;
  }
  return &signals_[index];
}

void Project::SortSignals() {
  std::ranges::sort(signals_, SignalLess);
}

void Project::DeleteSignal(std::string name) {
  std::erase_if(signals_, [&](const auto& signal) -> bool {
    return signal.GetName() == name;
  });
}

void Project::SetMasterReference(double value) {
  master_value_ = value;
}

double Project::GetMasterReference() const {
  return master_value_;
}

std::string Project::GetMasterName() const {
  if (const Signal* signal = GetSignal(0); signal != nullptr) {
    return signal->GetMasterName();
  }
  return {};
}

std::string Project::GetMasterUnit() const {
  if (const Signal* signal = GetSignal(0); signal != nullptr) {
    return signal->GetMasterUnit();
  }
  return {};
}
} // plot