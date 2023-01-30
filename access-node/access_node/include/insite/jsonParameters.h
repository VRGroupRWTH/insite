#pragma once
#include <cpr/cpr.h>
#include <crow.h>
#include <spdlog/spdlog.h>
#include <charconv>
#include <iostream>
#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>

#include "jsonStrings.h"
#include "queryStringBuilder.h"
#include "spike.h"

namespace insite {
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

template <typename T>
std::optional<T> GetParam(const crow::query_string& query_string,
                          const char* name) {
  std::optional<T> result;

  if (query_string.get(name) != nullptr) {
    result = ConvertStringToType<T>(query_string.get(name));
    SPDLOG_DEBUG("{}: {}", name, result.value());
  } else {
    SPDLOG_DEBUG("{}: no value", name);
  }

  return result;
}
template <typename T>
std::optional<T> GetParam(const crow::request& req, const char* name) {
  crow::query_string params = req.url_params;
  std::optional<T> result;

  if (params.get(name) != nullptr) {
    result = ConvertStringToType<T>(params.get(name));
    SPDLOG_DEBUG("{}: {}", name, result.value());
  } else {
    SPDLOG_DEBUG("{}: no value", name);
  }

  return result;
}

template <typename T>
std::unordered_set<T> GetParamUSet(const crow::query_string& query_string,
                                   const char* name) {
  std::unordered_set<T> result;

  if (!query_string.get_list(name, false).empty()) {
    std::vector<char*> value_strings = query_string.get_list(name, false);
    for (char* value_string : value_strings) {
      T value = ConvertStringToType<T>(value_string);
      result.insert(value);
    }
  }

  return result;
}

std::unordered_set<std::uint64_t> CommaListToSet(const char* input_string);

template <typename T>
std::unordered_set<T> GetParamUSet(const crow::request& req, const char* name) {
  crow::query_string params = req.url_params;
  std::unordered_set<T> parameter_set_as_type;
  auto param_list = params.get_list(name, false);

  if (param_list.size() > 1) {
    std::vector<char*> value_strings = params.pop_list(name, false);
    for (char* value_string : value_strings) {
      SPDLOG_INFO("GetParamUSet: {}", value_string);
      T value_as_type = ConvertStringToType<T>(value_string);
      parameter_set_as_type.insert(value_as_type);
    }
  } else if (param_list.size() == 1) {
    std::string param_list_string(*param_list.data());
    if (param_list_string.find_first_of(',') != std::string::npos) {
      return CommaListToSet(param_list_string.c_str());
    }
  }

  return parameter_set_as_type;
}

}  // namespace insite
