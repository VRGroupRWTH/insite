#pragma once
#include "crow/app.h"
#include "crow/http_response.h"
#include "crow/middlewares/cors.h"
#include "query_params.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "tvb_handler.h"
#include "utilityFunctions.h"

namespace insite {
class TVBHttpEndpoint {
private:
public:
  inline static TvbHandler *tvb_handler = nullptr;

  static void RegisterRoutes(crow::App<crow::CORSHandler> &app) {
    CROW_ROUTE(app, "/tvb/data")(&TVBHttpEndpoint::GetData);
    CROW_ROUTE(app, "/tvb/monitors")(&TVBHttpEndpoint::GetMonitors);
  }

  static crow::response GetMonitors(const crow::request& request) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    tvb_handler->SerializeMonitorsJson(writer);
    return {buffer.GetString()};
  }

  static crow::response GetData(const crow::request& request) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto query_params = TvbParameters(request.url_params);

    SPDLOG_DEBUG(
        "Quering TVB Monitor with internal id {}, uid: {}, fromTime: {}, "
        "toTime: {}",
        query_params.internal_id.value_or(9999),
        query_params.uid.value_or("no val"), query_params.from_time.value_or(0),
        query_params.to_time.value_or(std::numeric_limits<double>::max()));

    writer.StartArray();
    for (auto& monitor : tvb_handler->double_monitors_) {
      if (query_params["uid"] &&
              monitor.uid != query_params.GetValueAsT<std::string>("uid") ||
          query_params.HasValue("internalId") &&
              monitor.internal_id !=
                  query_params.GetValueAsT<int>("interalId")) {
        continue;
      }

      monitor.SerializeDataToJson(writer, query_params["fromTime"],
                                  query_params["toTime"]);
    }
    writer.EndArray();
    return {buffer.GetString()};
  }
};
}  // namespace insite
