#include <query_string_builder.h>

namespace insite {

// Builds a query-String that can be sent to the nest-sever by combining all the
// given attributes with all the given values
std::string BuildQueryString(const std::string& prefix, const std::vector<Parameter>& params) {
  std::string result = prefix + "?";
  for (auto it = params.begin(); it != params.end(); ++it) {
    if (it != params.begin()) {
      result += "&";
    }
    result += (it->first + "=" + it->second);
  }

  return result;
}

}