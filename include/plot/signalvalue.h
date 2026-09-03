/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

namespace plot {

class SignalValue {
public:
 SignalValue() = default;
 SignalValue(double value, double master_value, bool valid = true);
 SignalValue(const SignalValue& value);

 SignalValue& operator=(const SignalValue& value);

 void SetUpdated(bool updated) const { updated_ = updated; }
 [[nodiscard]] bool IsUpdated() const { return updated_; }

 void SetValid(bool valid) { valid_ = valid; }
 [[nodiscard]] bool IsValid() const { return valid_; }

 void SetValue(double value) { value_ = value; }
 [[nodiscard]] double GetValue() const { return value_; }

 void SetMasterValue(double value) { master_value_ = value; }
 [[nodiscard]] double GetMasterValue() const { return master_value_; }

 void SetValue(double value, double master_value, bool valid = true);
 [[nodiscard]] bool GetValue(double &value, double &master_value) const;

private:
 mutable bool updated_ = false;
 bool valid_ = false; ///< Indicate if the value is valid or not.
 double value_ = 0.0; ///< Y-Value.
 double master_value_ = 0.0; ///< X-axis value


};

} // namespace plot


