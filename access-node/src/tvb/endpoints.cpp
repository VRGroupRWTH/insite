#include <tvb/endpoints.h>

namespace insite {

void TVBHttpEndpoint::RegisterRoutes(crow::App<crow::CORSHandler> &app) {
    CROW_ROUTE(app, "/tvb/data")(&TVBHttpEndpoint::GetData);
    CROW_ROUTE(app, "/tvb/monitors")(&TVBHttpEndpoint::GetMonitors);
    CROW_ROUTE(app, "/tvb/exec/")(&TVBHttpEndpoint::LOL);
    CROW_ROUTE(app, "/tvb/simulation_info/")(&TVBHttpEndpoint::GetSimInfo);
    CROW_ROUTE(app, "/tvb/simulation_info/<path>")
    (&TVBHttpEndpoint::GetSimInfoDetails);
}


crow::response TVBHttpEndpoint::LOL(const crow::request &request) {
    if (tvb_handler->srv != nullptr) {
        auto query_string = GetQueryString(request.raw_url).substr(1);
        tvb_handler->srv->BroadcastAll(query_string, ResourceFlag::kTVB);
    } else {
        spdlog::error("Websocket Server in TVB Handler not set!");
    }
    return {""};
}


crow::response TVBHttpEndpoint::GetSimInfoDetails(const crow::request &request, const std::string &value) {
    if (tvb_handler->srv != nullptr) {
      tvb_handler->srv->BroadcastAll(value, ResourceFlag::kTVB);
      tvb_handler->sim_info_promise = std::promise<std::string>();
      auto future = tvb_handler->sim_info_promise.get_future();
      auto result = future.wait_for(std::chrono::milliseconds(500));

      if (result == std::future_status::timeout) {
        return {"timeout..."};
      }

      auto value = future.get();
      if (value == "null") {
        return {crow::status::BAD_REQUEST,
                R"({"error":"attribute not found"})"};
      }
      return {value};
    } else {
      spdlog::error("Websocket Server in TVB Handler not set!");
      return {"error"};
    }
}

crow::response TVBHttpEndpoint::GetSimInfo(const crow::request &request) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    tvb_handler->sim_info.Accept(writer);

    return {buffer.GetString()};
}

crow::response TVBHttpEndpoint::GetMonitors(const crow::request &request) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    tvb_handler->SerializeMonitorsJson(writer);
    return {buffer.GetString()};
}

crow::response TVBHttpEndpoint::GetData(const crow::request &request) {
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
    for (auto &monitor : tvb_handler->double_monitors_) {
      if ((query_params.uid && monitor.uid != query_params.uid) ||
          (query_params.internal_id &&
           monitor.internal_id != query_params.internal_id)) {
        continue;
      }

      monitor.SerializeDataToJson(writer, query_params.from_time,
                                  query_params.to_time);
    }
    writer.EndArray();
    return {buffer.GetString()};
}

}