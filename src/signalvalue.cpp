/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "plot/signalvalue.h"

namespace plot {

SignalValue::SignalValue(SignalValueType type) : type_(type) {}

SignalValue::SignalValue(double value, double master_value, bool valid) {
  type_ = SignalValueType::kFloat;
  SetValue(value, master_value, valid);
}


SignalValue::SignalValue(const SignalValue& value) :
type_(value.type_),
updated_(true),
valid_(value.valid_),
value_(value.value_),
master_value_(value.master_value_)
{
}

SignalValue& SignalValue::operator=(const SignalValue& value) {
 if (this == &value) {
  return *this;
 }
 type_ = value.type_;
 valid_ = value.valid_;
 value_ = value.value_;
 master_value_ = value.master_value_;
 updated_ = true;
 return *this;
}

bool SignalValue::operator<(const SignalValue& value) const {
  try {
    switch (type_) {
      case SignalValueType::kInt:
        return std::any_cast<int64_t>(value_) <
           std::any_cast<int64_t>(value.value_);

     case SignalValueType::kUint:
        return std::any_cast<uint64_t>(value_) <
          std::any_cast<uint64_t>(value.value_);

      case SignalValueType::kFloat:
        return std::any_cast<double>(value_) <
          std::any_cast<double>(value.value_);

      case SignalValueType::kString:
        return std::any_cast<std::string>(value_) <
          std::any_cast<std::string>(value.value_);

      case SignalValueType::kBoolean:
        return std::any_cast<bool>(value_) <
          std::any_cast<bool>(value.value_);

      default:
        break;
    }
  } catch (const std::exception&) {

  }

  return false;
}





} // plot