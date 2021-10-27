#include "serialize.hpp"

#include "stringdatum.h"
#include "booldatum.h"
#include "integerdatum.h"
#include "doubledatum.h"
#include "namedatum.h"
#include "arraydatum.h"
#include "dictdatum.h"
#include <cmath>


namespace insite {

web::json::value SerializeDatum(Datum* datum) {
  if (IntegerDatum* integer_datum = dynamic_cast<IntegerDatum*>(datum)) {
    return web::json::value::number(*integer_datum);
  } else if (DoubleDatum* double_datum = dynamic_cast<DoubleDatum*>(datum)) {
    if(std::isfinite(*double_datum)) {
        return web::json::value::number(*double_datum);
    }else{
        return web::json::value::Null;
    }
  } else if (StringDatum* string_datum = dynamic_cast<StringDatum*>(datum)) {
    return web::json::value::string(*string_datum);
  } else if (NameDatum* name_datum = dynamic_cast<NameDatum*>(datum)) {
    return web::json::value::string(name_datum->toString());
  } else if (BoolDatum* bool_datum = dynamic_cast<BoolDatum*>(datum)) {
    return web::json::value::boolean(*bool_datum);
  } else if (ArrayDatum* array_datum = dynamic_cast<ArrayDatum*>(datum)) {
    web::json::value array = web::json::value::array();
    for (const auto& datum : *array_datum) {
      array[array.size()] = SerializeDatum(datum.datum());
    }
    return array;
  } else if (DictionaryDatum* dictionary_datum = dynamic_cast<DictionaryDatum*>(datum)) {
    web::json::value object = web::json::value::object();
    for (const auto& datum : **dictionary_datum) {
      object[datum.first.toString()] = SerializeDatum(datum.second.datum());
    }
    return object;
  } else {
    return web::json::value::null();
  }
}

}  // namespace insite
