#pragma once

#include <cpprest/json.h>
#include <datum.h>

namespace insite {

web::json::value SerializeDatum(Datum* datum);
inline web::json::value SerializeDatum(Datum& datum) { return SerializeDatum(&datum); }

}  // namespace insite
