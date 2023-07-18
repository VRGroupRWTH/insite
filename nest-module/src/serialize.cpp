#include "serialize.hpp"

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <cmath>
#include <string>

#include <spdlog/spdlog.h>
#include "arraydatum.h"
#include "booldatum.h"
#include "dictdatum.h"
#include "doubledatum.h"
#include "integerdatum.h"
#include "namedatum.h"
#include "rapidjson/document.h"
#include "spdlog/spdlog.h"
#include "stringdatum.h"

namespace insite {

void SerializeDatum(Datum* datum, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
  if (IntegerDatum* integer_datum = dynamic_cast<IntegerDatum*>(datum)) {
    writer.Int(*integer_datum);
  } else if (DoubleDatum* double_datum = dynamic_cast<DoubleDatum*>(datum)) {
    if (std::isfinite(*double_datum)) {
      writer.Double(*double_datum);
    } else {
      writer.Null();
    }
  } else if (StringDatum* string_datum = dynamic_cast<StringDatum*>(datum)) {
    writer.String(string_datum->data(), string_datum->size(), true);
  } else if (NameDatum* name_datum = dynamic_cast<NameDatum*>(datum)) {
    writer.String(name_datum->toString().c_str(), name_datum->toString().size(), true);
  } else if (LiteralDatum* literal_datum = dynamic_cast<LiteralDatum*>(datum)) {
    writer.String(literal_datum->toString().c_str());
  } else if (BoolDatum* bool_datum = dynamic_cast<BoolDatum*>(datum)) {
    writer.Bool(*bool_datum);
  } else if (ArrayDatum* array_datum = dynamic_cast<ArrayDatum*>(datum)) {
    writer.StartArray();
    for (const auto& datum : *array_datum) {
      SerializeDatum(datum.datum(), writer);
    }
    writer.EndArray();
  } else if (DictionaryDatum* dictionary_datum = dynamic_cast<DictionaryDatum*>(datum)) {
    writer.StartObject();
    for (const auto& datum : **dictionary_datum) {
      writer.Key(datum.first.toString().c_str());
      SerializeDatum(datum.second.datum(), writer);
    }
    writer.EndObject();
  } else {
    spdlog::error("Couldnt cast Datum: {}", datum->gettypename().toString());
    writer.Null();
  }
}

// web::json::value SerializeDatum(Datum* datum) {
//   if (IntegerDatum* integer_datum = dynamic_cast<IntegerDatum*>(datum)) {
//     return web::json::value::number(*integer_datum);
//   } else if (DoubleDatum* double_datum = dynamic_cast<DoubleDatum*>(datum)) {
//     if (std::isfinite(*double_datum)) {
//       return web::json::value::number(*double_datum);
//     } else {
//       return web::json::value::Null;
//     }
//   } else if (StringDatum* string_datum = dynamic_cast<StringDatum*>(datum)) {
//     return web::json::value::string(*string_datum);
//   } else if (NameDatum* name_datum = dynamic_cast<NameDatum*>(datum)) {
//     return web::json::value::string(name_datum->toString());
//   } else if (BoolDatum* bool_datum = dynamic_cast<BoolDatum*>(datum)) {
//     return web::json::value::boolean(*bool_datum);
//   } else if (ArrayDatum* array_datum = dynamic_cast<ArrayDatum*>(datum)) {
//     web::json::value array = web::json::value::array();
//     for (const auto& datum : *array_datum) {
//       array[array.size()] = SerializeDatum(datum.datum());
//     }
//     return array;
//   } else if (DictionaryDatum* dictionary_datum = dynamic_cast<DictionaryDatum*>(datum)) {
//     web::json::value object = web::json::value::object();
//     for (const auto& datum : **dictionary_datum) {
//       object[datum.first.toString()] = SerializeDatum(datum.second.datum());
//     }
//     return object;
//   } else {
//     return web::json::value::null();
//   }
// }

}  // namespace insite
