#include <query_string_builder.h>

namespace insite {

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