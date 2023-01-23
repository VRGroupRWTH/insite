#pragma once
#include <string>
#include <unordered_set>
#include <vector>

namespace insite {
using Parameter = std::pair<std::string, std::string>;

// Builds a query-String that can be sent to the nest-sever by combining all the
// given attributes with all the given values
inline std::string BuildQueryString(const std::string& prefix,
                                    const std::vector<Parameter>& params) {
  std::string result = prefix + "?";
  for (auto it = params.begin(); it != params.end(); ++it) {
    if (it != params.begin()) {
      result += "&";
    }
    result += (it->first + "=" + it->second);
  }

  return result;
}

template <typename T>
std::string VectorToCsv(const std::vector<T>& values) {
  std::string ret;

  for (auto it = values.begin(); it != values.end(); ++it) {
    if (it != values.begin()) {
      ret += ",";
    }
    ret += std::to_string(*it);
  }

  return ret;
}

template <typename T>
std::string UnorderedSetToCsv(const std::unordered_set<T>& values) {
  std::string ret;

  for (auto it = values.begin(); it != values.end(); ++it) {
    if (it != values.begin()) {
      ret += ",";
    }
    ret += std::to_string(*it);
  }

  return ret;
}
}  // namespace insite
