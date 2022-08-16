#pragma once

#include <cpprest/json.h>
#include <datum.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <vector>

namespace insite {

web::json::value SerializeDatum(Datum* datum);
inline web::json::value SerializeDatum(Datum& datum) {
  return SerializeDatum(&datum);
}

void SerializeDatum(Datum* datum, rapidjson::Writer<rapidjson::StringBuffer>& writer);
inline void SerializeDatum(Datum& datum, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
  return SerializeDatum(&datum, writer);
}

template <typename T>
inline web::json::value ToJsonArray(const std::vector<T>& vector) {
  web::json::value array = web::json::value::array(vector.size());

  for (size_t i = 0; i < vector.size(); ++i) {
    array[i] = vector[i];
  }

  return array;
}
}  // namespace insite
