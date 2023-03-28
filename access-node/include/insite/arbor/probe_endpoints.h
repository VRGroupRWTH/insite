#pragma once
#include "crow/http_response.h"

namespace insite::arbor {

crow::response GetProbeData(const crow::request &req);
crow::response GetProbes(const crow::request &req);
} // namespace insite::arbor
