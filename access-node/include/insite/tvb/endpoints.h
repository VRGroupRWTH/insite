#pragma once
#include <tvb/handler.h>
#include <utility_functions.h>
#include <chrono>
#include "crow/app.h"
#include "crow/http_response.h"
#include "crow/middlewares/cors.h"
#include "query_params.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace insite {
class TVBHttpEndpoint {
 private:
 public:
  inline static TvbHandler* tvb_handler = nullptr;

  static void RegisterRoutes(crow::App<crow::CORSHandler>& app);

  static crow::response GetSimInfoDetails(const crow::request& request, const std::string& value);

  static crow::response GetSimInfo(const crow::request& request);

  static crow::response GetMonitors(const crow::request& request);

  static crow::response GetData(const crow::request& request);
};
}  // namespace insite
