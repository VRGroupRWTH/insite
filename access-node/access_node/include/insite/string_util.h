#pragma once

#include <string>
template <typename T>
T ConvertStringToType(const char* input_string);

template <>
inline bool ConvertStringToType<bool>(const char* input_string) {
  return std::stoi(input_string) != 0;
}
template <>
inline int ConvertStringToType<int>(const char* input_string) {
  return std::stoi(input_string);
}

template <>
inline uint64_t ConvertStringToType(const char* input_string) {
  return std::stoull(input_string);
}

template <>
inline double ConvertStringToType(const char* input_string) {
  return std::stod(input_string);
}
template <>
inline std::string ConvertStringToType(const char* input_string) {
  return input_string;
}
