#pragma once
#include <crow/http_response.h>
#include <rapidjson/document.h>

namespace insite {
rapidjson::Value NestGetSimulationTimeInfo();

rapidjson::Value NestGetVersion();
//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response SimulationTimeInfo();
crow::response Version();
}  // namespace insite
