#pragma once
#include <better_params.h>
#include "crow/app.h"
#include "crow/http_response.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "tvb_handler.h"

namespace insite {
class TVBHttpEndpoint {
 private:
 public:
  inline static TvbHandler* tvb_handler = nullptr;

  static void RegisterRoutes(crow::SimpleApp& app) {
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

    auto query_params =
        betterinsite::TVBDataQueryParameters(request.url_params);

    SPDLOG_DEBUG("fromTime: {}, toTime: {}",
                 query_params["fromTime"].ValueAsTOrDefault<double>(),
                 query_params["toTime"].ValueAsTOrDefault<double>());
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
