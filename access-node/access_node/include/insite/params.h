#pragma once
#include <crow/http_request.h>
#include <crow/query_string.h>
#include <spdlog/spdlog.h>
#include <array>
#include <string>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include <jsonParameters.h>
#include <jsonStrings.h>

// template <std::size_t N, typename... Types>
// using NthParameterT = std::tuple_element_t<N, std::tuple<Types...>>;

namespace insite {

template <class>
inline constexpr bool kAlwaysFalseV = false;
using ParameterValue = std::variant<std::string,
                                    bool,
                                    int,
                                    double,
                                    uint64_t,
                                    std::unordered_set<uint64_t>>;
using OptionalParameterValue = std::optional<ParameterValue>;
using ParameterName = std::string;

struct Param {
  ParameterName name;
  ParameterValue value;

  [[nodiscard]] std::string KeyValueToString() const;
  [[nodiscard]] ParameterName Name() const { return name; };
  [[nodiscard]] ParameterValue Value() const { return value; };
  template <typename T>
  [[nodiscard]] T ValueAsT() const { return std::get<T>(value); };
};

struct OptionalParameter {
  ParameterName name;
  OptionalParameterValue value;

  template <class T>
  [[nodiscard]] static OptionalParameter GetFromQueryString(const crow::query_string& query,
                                              const char* name) {
    std::optional<T> result;

    if (query.get(name) != nullptr) {
      result = ConvertStringToType<T>(query.get(name));
      SPDLOG_DEBUG("{}: {}", name, result.value());
    } else {
      SPDLOG_DEBUG("{}: no value", name);
    }

    return {name, result};
  }

  template <class T>
  [[nodiscard]] inline static OptionalParameter GetFromQueryString(
      const crow::request& req,
      const char* name) {
    return OptionalParameter::GetFromQueryString<T>(req.url_params, name);
  };

  template <class T>
  [[nodiscard]] static OptionalParameter GetUSetFromQueryString(
      const crow::query_string& query_string,
      const char* name) {
    std::unordered_set<T> result;

    if (!query_string.get_list(name, false).empty()) {
      std::vector<char*> value_strings = query_string.get_list(name, false);
      for (char* value_string : value_strings) {
        T value = ConvertStringToType<T>(value_string);
        result.insert(value);
      }
      return {name, result};
    }
    return {name, std::nullopt};
  }

  template <class T>
  [[nodiscard]] inline static OptionalParameter GetUSetFromQueryString(
      const crow::request& req,
      const char* name) {
    return GetUSetFromQueryString<T>(req.url_params, name);
  }

  //
  [[nodiscard]] std::string KeyValueToString() const;

  [[nodiscard]] ParameterValue Value() { return value.value(); };

  template <class T>
  [[nodiscard]] T ValueAsT() { return std::get<T>(value.value()); };
  
  template <class T>
  [[nodiscard]] T ValueAsTOr(T default_value) { return value.has_value() ? std::get<T>(value.value()) : default_value; };

  [[nodiscard]] bool HasValue() const { return value.has_value(); };
  
  operator bool() const { return value.has_value(); }
};

struct OptionalParameters {
 public:
  // std::vector<OptionalParameter> parameters;
  std::unordered_map<ParameterName, OptionalParameter> parameters;

  [[nodiscard]] std::vector<ParameterName> Keys();
  [[nodiscard]] std::vector<ParameterValue> Values();

  [[nodiscard]] OptionalParameter GetValue(const ParameterName& parameter_name);

  [[nodiscard]] std::string ToQueryString();
  [[nodiscard]] static std::string ToQueryString(
      const std::vector<OptionalParameter>& parameters);
};

struct MultimeterParameters : public OptionalParameters {
  MultimeterParameters(const crow::request& request) {
    using namespace insite::json_strings;

    parameters.insert({kFromTime, OptionalParameter::GetFromQueryString<double>(
                                      request.url_params, kFromTime)});
    parameters.insert({kToTime, OptionalParameter::GetFromQueryString<double>(
                                    request, kToTime)});
    parameters.insert(
        {kNodeIds, OptionalParameter::GetUSetFromQueryString<uint64_t>(
                       request, kNodeIds)});
    parameters.insert({kSkip, OptionalParameter::GetFromQueryString<uint64_t>(
                                  request, kSkip)});
    parameters.insert(
        {kTop, OptionalParameter::GetFromQueryString<uint64_t>(request, kTop)});
    parameters.insert(
        {kSort,
         OptionalParameter::GetFromQueryString<std::string>(request, kSort)});
  }
};

// class Parameters {
//  public:
//   Parameters(const crow::request& request) : req(request){};
//   using Variant = std::variant<bool, int, double,

//   std::unordered_set<uint64_t>>; template <typename... Types> void
//   ReadParams(std::array<std::string, sizeof...(Types)> names) {
//     ReadParams<Types...>(names,
//     std::make_index_sequence<sizeof...(Types)>());
//   }
//
//   template <typename T>
//   std::optional<T> GetParam(std::string key) {
//     if (m_[key])
//       return std::optional<T>(std::get<T>(m_[key]));
//     else
//       return std::optional<T>();
//   }
//
//  private:
//   template <typename... Types, size_t... I>
//   void ReadParams(std::array<std::string, sizeof...(Types)> names,
//                   std::index_sequence<I...>) {
//     (m_.insert(
//          {names[I], GetParamFromReq<NthParameterT<I,
//          Types...>>(names[I])}),
//      ...);
//     // (m.insert({names[I], convertToType<nth_parameter_t<I,
//     Types...>>("0")}),
//     // ...);
//   }
//
//   template <typename T>
//   T GetParamFromReq(std::string param_name) {
//     T value;
//     const char* string_val = req_.url_params.get(param_name);
//     if (string_val == nullptr)
//       return std::optional<T>();
//     std::stringstream ss(string_val);
//     ss >> value;
//     return value;
//   }
//   template <>
//   std::unordered_set<uint64_t> GetParamFromReq(std::string param_name) {
//     std::unordered_set<uint64_t> value;
//     auto string_val = req_.url_params.get_list(param_name);
//     for (const auto& val : string_val) {
//       value.insert(std::stoull(val));
//     }
//     return value;
//   }
//   const crow::request& req_;
//   std::unordered_map<std::string, Variant> m_;
// };
}  // namespace insite
