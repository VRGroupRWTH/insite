#pragma once
#include <insite/arbor/pipeline_backend.h>
#include <tl/optional.hpp>

namespace insite {

template <typename T>
inline bool ValueInListIfListExists(T value,
                                    tl::optional<std::vector<T>>& list) {
  return (!list || std::find(list->begin(), list->end(),
                             static_cast<T>(value)) != list->end());
}

template <typename T>
bool ValueInList(const tl::optional<std::vector<T>>& list, const T value) {
  return (!list || std::find(list->begin(), list->end(),
                             static_cast<T>(value)) != list->end());
}

template <typename T>
bool ValueInList(std::vector<T>& list, T value) {
  return (std::find(list.begin(), list.end(), static_cast<T>(value)) !=
          list.end());
}

template <typename T>
bool ValueInOptionalRange(T value, tl::optional<T> from, tl::optional<T> to) {
  bool in_time_range_lower = from.has_value() ? from.value() <= value : true;

  bool in_time_range_upper = to.has_value() ? to.value() >= value : true;

  return in_time_range_lower && in_time_range_upper;
}

}  // namespace insite
