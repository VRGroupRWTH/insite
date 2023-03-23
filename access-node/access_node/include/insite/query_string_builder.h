#pragma once
#include <string>
#include <unordered_set>
#include <vector>

namespace insite {
using Parameter = std::pair<std::string, std::string>;

// Builds a query-String that can be sent to the nest-sever by combining all the
// given attributes with all the given values
std::string BuildQueryString(const std::string& prefix, const std::vector<Parameter>& params);

template <typename T>
std::string VectorToCsv(const std::vector<T>& values);

template <typename T>
std::string UnorderedSetToCsv(const std::unordered_set<T>& values);

}  // namespace insite

#include "../../src/query_string_builder.inl"
