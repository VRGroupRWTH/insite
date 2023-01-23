#include "params.h"
#include <optional>
#include <unordered_set>
#include <vector>
#include "crow/query_string.h"
#include "queryStringBuilder.h"

namespace insite {
template <typename>
struct IsStdUSet : std::false_type {};

template <typename T, typename A>
struct IsStdUSet<std::unordered_set<T, A>> : std::true_type {};
std::string Param::KeyValueToString() const {
  std::string res = name;

  std::visit(
      [&res, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          res += name + "=" + arg;
        } else if constexpr (std::is_arithmetic_v<T>) {
          res += name + "=" + std::to_string(arg);
        } else if constexpr (IsStdUSet<T>::value) {
        } else {
          static_assert(kAlwaysFalseV<T>, "non-exhaustive visitor!");
        }
      },
      value);

  return res;
}

std::string OptionalParameter::KeyValueToString() const {
  std::string res = name + "=";
  if (!value) {
    return "";
  }

  std::visit(
      [&res, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          res += arg;
        } else if constexpr (std::is_arithmetic_v<T>) {
          res += std::to_string(arg);
        } else if constexpr (IsStdUSet<T>::value) {
          res += UnorderedSetToCsv(arg);
        } else {
          static_assert(kAlwaysFalseV<T>, "non-exhaustive visitor!");
        }
      },
      value.value());

  return res;
}

std::string OptionalParameters::ToQueryString(
    const std::vector<OptionalParameter>& parameters) {
  std::string result = "?";
  for (auto it = parameters.begin(); it != parameters.end(); ++it) {
    if (*it) {
      if (it != parameters.begin()) {
        result += "&";
      }
      result += it->KeyValueToString();
    }
  }

  return result;
}

std::string OptionalParameters::ToQueryString() {
  std::string result = "?";
  for (auto it = parameters.begin(); it != parameters.end(); ++it) {
    if (it != parameters.begin()) {
      result += "&";
    }
    if (it->second) {
      result += it->second.KeyValueToString();
    }
  }

  return result;
}

// Returns the parameter value if it is present nullopt otherwise
OptionalParameter OptionalParameters::GetValue(
    const ParameterName& parameter_name) {
  if (auto parameter = parameters.find(parameter_name);
      parameter != parameters.end()) {
    return parameter->second;
  }

  return {parameter_name, std::nullopt};
}

}  // namespace insite
