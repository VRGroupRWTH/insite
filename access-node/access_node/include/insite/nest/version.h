#pragma once
#include <crow/http_response.h>
#include <rapidjson/document.h>

namespace insite {

rapidjson::Value NestGetVersion();
//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response SimulationTimeInfo(int api_version);
crow::response Version();
} // namespace insite
