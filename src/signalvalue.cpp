/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "plot/signalvalue.h"

namespace plot {

SignalValue::SignalValue(double value, double master_value, bool valid) {
  SetValue(value, master_value, valid);
}


SignalValue::SignalValue(const SignalValue& value) {
 // Note that the updated flag is not copied
 SetValue(value.value_, value.master_value_, value.valid_);
}

SignalValue& SignalValue::operator=(const SignalValue& value) {
 if (this == &value) {
  return *this;
 }
 SetValue(value.value_, value.master_value_, value.valid_);
 return *this;
}

void SignalValue::SetValue(double value, double master_value, bool valid) {
 value_ = value;
 master_value_ = master_value;
 valid_ = valid;
 updated_ = true;
}

bool SignalValue::GetValue(double &value, double &master_value) const {
 value = value_;
 master_value = master_value_;
 updated_ = false;
 return valid_;
}

} // plot