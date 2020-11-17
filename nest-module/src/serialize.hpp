#pragma once

#include <cpprest/json.h>
#include <datum.h>

namespace insite {

web::json::value SerializeDatum(Datum* datum);

}  // namespace insite
