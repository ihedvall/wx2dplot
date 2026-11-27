/*
* Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <any>
#include <type_traits>
#include <string>
#include <cstdint>
#include <vector>

namespace plot {

enum class SignalValueType {
  kUndefined = 0,
  kInt = 1,
  kUint = 2,
  kFloat = 3,
  kString = 4,
  kByteArray = 6,
  kBoolean = 7,
};

class SignalValue {
public:
  SignalValue() = default;
  explicit SignalValue(SignalValueType type);
  SignalValue(double value, double master_value, bool valid = true);
  SignalValue(const SignalValue& value);

  SignalValue& operator= (const SignalValue& value);
  bool operator< (const SignalValue& value) const;

  void SetUpdated(bool updated) const { updated_ = updated; }
  [[nodiscard]] bool IsUpdated() const { return updated_; }

  void SetValid(bool valid) { valid_ = valid; }
  [[nodiscard]] bool IsValid() const { return valid_; }

  void SetDataType(SignalValueType type) { type_ = type; }
  [[nodiscard]] SignalValueType GetDataType() const { return type_; }

  template <typename T>
  void SetValue(T value);

  template <typename T>
  [[nodiscard]] T GetValue() const;

  void SetMasterValue(double value) { master_value_ = value; }
  [[nodiscard]] double GetMasterValue() const { return master_value_; }

  template<typename T>
  void SetValue(T value, double master_value, bool valid = true);

  template<typename T>
  [[nodiscard]] bool GetValue(T &value, double &master_value) const;

private:
  SignalValueType type_ = SignalValueType::kUndefined;
  mutable bool updated_ = false;
  bool valid_ = false; ///< Indicate if the value is valid or not.
  std::any value_ = 0.0; ///< Y-Value.
  double master_value_ = 0.0; ///< X-axis value
};

template <typename T>
void SignalValue::SetValue(T value) {
  switch (type_) {
    case SignalValueType::kInt:
      value_ = static_cast<int64_t>(value);
      break;

    case SignalValueType::kUint:
      value_ = static_cast<uint64_t>(value);
      break;

    case SignalValueType::kFloat:
      value_ = static_cast<double>(value);
      break;

    case SignalValueType::kByteArray:
    case SignalValueType::kString:
      value_ = value;
      break;

    case SignalValueType::kBoolean:
      value_ = static_cast<bool>(value);
      break;

    case SignalValueType::kUndefined:
    default:
      if (std::is_same_v<bool, T>) {
        type_ = SignalValueType::kInt;
        value_ = value > 0;
      } else if (std::is_integral_v<T> && std::is_signed_v<T>) {
        type_ = SignalValueType::kInt;
        value_ = static_cast<int64_t>(value);
      } else if (std::is_integral_v<T> && std::is_unsigned_v<T>) {
        type_ = SignalValueType::kUint;
        value_ = static_cast<uint64_t>(value);
      } else if (std::is_floating_point_v<T>) {
        type_ = SignalValueType::kFloat;
        value_ = static_cast<double>(value);
      } else if (std::is_same_v<std::string,T>) {
        type_ = SignalValueType::kString;
        value_ = std::move(value);
      } else if (std::is_same_v<std::vector<uint8_t>,T>) {
        type_ = SignalValueType::kByteArray;
        value_ = std::move(value);
      } else {
        value_ = value;
      }
      break;
  }
}

template <typename T>
T SignalValue::GetValue() const {
  constexpr T value = {};
  try {
    return std::any_cast<T>(value_);
  } catch (const std::exception&) {
  }
  return value;
}

template <typename T>
void SignalValue::SetValue(T value, double master_value, bool valid) {
  SetValue(value);
  master_value_ = master_value;
  valid_ = valid;
  updated_ = true;
}

template <typename T>
bool SignalValue::GetValue(T &value, double &master_value) const {
  GetValue(value);
  master_value = master_value_;
  updated_ = false;
  return valid_;
}
} // namespace plot


