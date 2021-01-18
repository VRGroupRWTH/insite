#pragma once

#include <vector>

#include <cpprest/json.h>
#include <datum.h>

namespace insite {

web::json::value SerializeDatum(Datum* datum);
inline web::json::value SerializeDatum(Datum& datum) { return SerializeDatum(&datum); }

template <typename T>
inline web::json::value ToJsonArray(const std::vector<T>& vector) {
  web::json::value array = web::json::value::array(vector.size());

  for (size_t i = 0; i < vector.size(); ++i) {
    array[i] = vector[i];
  }

  return array;
}

}  // namespace insite
