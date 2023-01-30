#pragma once
#include <crow/http_request.h>
#include <crow/query_string.h>
#include <spdlog/spdlog.h>
#include <array>
#include <limits>
#include <optional>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include "jsonStrings.h"
// #include "jsonParameters.h"
// template <std::size_t N, typename... Types>
// using NthParameterT = std::tuple_element_t<N, std::tuple<Types...>>;

namespace betterinsite {

inline constexpr bool kAlwaysFalseV = false;
using ParameterValue = std::variant<std::string,
                                    bool,
                                    int,
                                    double,
                                    uint64_t,
                                    std::unordered_set<uint64_t>>;
using OptionalParameterValue = std::optional<ParameterValue>;
using ParameterName = std::string;

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
tl::optional<T> GetParameter(const crow::query_string& query_string,
                             const std::string& fieldname) {
  const auto parameter_value = query_string.get(fieldname);
  if (parameter_value == nullptr) {
    return tl::nullopt;
  }
  return ConvertStringToType<T>(parameter_value);
}
struct Param {
  ParameterName name;
  ParameterValue value;

  [[nodiscard]] std::string KeyValueToString() const;
  [[nodiscard]] ParameterName Name() const { return name; };
  [[nodiscard]] ParameterValue Value() const { return value; };
  template <typename T>
  [[nodiscard]] T ValueAsT() const {
    return std::get<T>(value);
  };
};

struct OptionalParameter {
 public:
  ParameterName name;
  std::optional<std::string> value;
  std::optional<ParameterValue> default_value;
  // OptionalParameterValue value;

  [[nodiscard]] static OptionalParameter GetFromQueryString(
      const crow::query_string& query,
      const char* name,
      const ParameterValue& default_value) {
    if (auto* parameter = query.get(name); parameter) {
      spdlog::debug("Parsed parameter {}: {}", name, parameter);
      return {name, std::string(parameter), default_value};
    }
    spdlog::debug("Parsed parameter {}: no value", name);
    return {name, std::nullopt, default_value};
  }

  [[nodiscard]] static OptionalParameter GetFromQueryString(
      const crow::request& req,
      const char* name,
      const ParameterValue& default_value) {
    return OptionalParameter::GetFromQueryString(req.url_params, name,
                                                 default_value);
  };

  [[nodiscard]] bool HasValue() const { return value.has_value(); };
  [[nodiscard]] ParameterValue Value() { return value.value(); };
  operator bool() const { return value.has_value(); }

  template <class T>
  [[nodiscard]] T ValueAsT() {
    return ConvertStringToType<T>(value.value().c_str());
  };

  template <class T>
  [[nodiscard]] T ValueAsTOr(T default_value) {
    return value.has_value() ? ConvertStringToType<T>(value.value().c_str())
                             : default_value;
  };

  template <class U>
  U ValueAsTOrDefault() {
    return value.has_value() ? ConvertStringToType<U>(value.value().c_str())
                             : std::get<U>(default_value.value());
  };
};

struct OptionalParameters {
 public:
  std::unordered_map<ParameterName, OptionalParameter> parameters;

  void AddFromQueryString(const crow::query_string& query,
                          const std::string& name,
                          const ParameterValue& default_value) {
    parameters.insert({name, OptionalParameter::GetFromQueryString(
                                 query, name.c_str(), default_value)});
  }

  [[nodiscard]] std::vector<ParameterName> Keys();
  [[nodiscard]] std::vector<ParameterValue> Values();

  [[nodiscard]] OptionalParameter Get(const ParameterName& parameter_name) {
    if (auto parameter = parameters.find(parameter_name);
        parameter != parameters.end()) {
      return parameter->second;
    }
    return {parameter_name, std::nullopt};
  };

  [[nodiscard]] std::string GetValue(const ParameterName& parameter_name) {
    if (auto parameter = parameters.find(parameter_name);
        parameter != parameters.end()) {
      if (parameter->second) {
        return parameter->second.value.value();
      }
    }
    return "";
  };

  [[nodiscard]] bool HasValue(const ParameterName& parameter_name) {
    if (auto parameter = parameters.find(parameter_name);
        parameter != parameters.end()) {
      return parameter->second;
    }
    return false;
  };

  template <typename T>
  [[nodiscard]] T GetValueAsT(const ParameterName& parameter_name) {
    if (auto parameter = parameters.find(parameter_name);
        parameter != parameters.end()) {
      if (parameter->second) {
        return parameter->second.ValueAsT<T>();
      }
    }
    return T{};
  };

  template <typename T>
  [[nodiscard]] T GetValueAsTOrDefault(const ParameterName& parameter_name) {
    if (auto parameter = parameters.find(parameter_name);
        parameter != parameters.end()) {
      if (parameter->second) {
        return parameter->second.ValueAsTOrDefault<T>();
      }
    }
    return T{};
  };

  template <typename T>
  [[nodiscard]] T GetValueAsTOr(const ParameterName& parameter_name,
                                T default_value) {
    if (auto parameter = parameters.find(parameter_name);
        parameter != parameters.end()) {
      if (parameter->second) {
        return parameter->second.ValueAsTOr<T>(default_value);
      }
    }
    return default_value;
  };

  [[nodiscard]] OptionalParameter operator[](const std::string& key) {
    if (auto parameter = parameters.find(key); parameter != parameters.end()) {
      return parameter->second;
    }
    return {key, std::nullopt};
  }

  [[nodiscard]] std::string ToQueryString();
  [[nodiscard]] static std::string ToQueryString(
      const std::vector<OptionalParameter>& parameters);
};

struct TVBDataQueryParameters : public OptionalParameters {
  TVBDataQueryParameters(const crow::query_string& query_string) {
    AddFromQueryString(query_string, "fromTime", 0.0);
    AddFromQueryString(query_string, "toTime",
                       std::numeric_limits<double>::max());
    AddFromQueryString(query_string, "internalId", 0);
    AddFromQueryString(query_string, "uid", "");
  }
};

// struct MultimeterParameters : public OptionalParameters {
//   MultimeterParameters(const crow::query_string& query_string) {
//     using namespace insite::json_strings;
//
//     AddFromQueryString(query_string, kFromTime, 0.0);
//     AddFromQueryString(query_string, kToTime,
//                        std::numeric_limits<double>::max());
//
//     parameters.insert(
//         {kNodeIds, OptionalParameter::GetUSetFromQueryString<uint64_t>(
//                        request, kNodeIds)});
//     parameters.insert({kSkip,
//     OptionalParameter::GetFromQueryString<uint64_t>(
//                                   request, kSkip)});
//     parameters.insert(
//         {kTop, OptionalParameter::GetFromQueryString<uint64_t>(request,
//         kTop)});
//     parameters.insert(
//         {kSort,
//          OptionalParameter::GetFromQueryString<std::string>(request,
//          kSort)});
//   }
// };

}  // namespace betterinsite
