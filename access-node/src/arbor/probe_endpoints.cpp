#include <insite_defines.h>

#include <arbor/arbor_strings.h>
#include <arbor/probe_endpoints.h>
#include <config.h>
#include <utility_functions.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "spdlog/spdlog.h"

namespace insite::arbor {

crow::response GetProbes(const crow::request& req) {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  SPDLOG_DEBUG("Called Arbor GetProbes");
  rapidjson::StringBuffer buffer;
  RapidjsonWriter writer(buffer);

  auto params = GetQueryString(req.raw_url);
  auto probe_data_responses = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_arbor_urls, kProbeEndpoint, params);

  rapidjson::Value cell_infos_json(rapidjson::kArrayType);

  SPDLOG_TRACE("Parsing {} responses", probe_data_responses.size());
  for (const auto& cell_info : probe_data_responses) {
    rapidjson::Document info;
    info.Parse(cell_info.text.c_str(), cell_info.text.size());

    SPDLOG_TRACE("Received data from one simulator node: {}", cell_info.text);
    for (auto& entry : info["probes"].GetArray()) {
      // cell_infos_json.PushBack(
      //     rapidjson::GenericValue(entry, cell_infos_json.GetAllocator()),
      //     cell_infos_json.GetAllocator());

      rapidjson::Value insert(entry, json_alloc);
      cell_infos_json.PushBack(insert, json_alloc);
    }
  }

  rapidjson::Document probe_response;
  probe_response.SetObject();
  probe_response.AddMember("probes", cell_infos_json, json_alloc);
  probe_response.Accept(writer);
  SPDLOG_TRACE("Resulting JSON: {}", buffer.GetString());

  return {std::string(buffer.GetString())};
}

crow::response GetProbeData(const crow::request& req) {
  SPDLOG_DEBUG("Called Arbor GetProbeData");
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::StringBuffer buffer;
  RapidjsonWriter writer(buffer);

  SPDLOG_DEBUG("Querying {} simulator instances",
               ServerConfig::GetInstance().request_arbor_urls.size());

  auto params = GetQueryString(req.raw_url);

  auto probe_data_responses =
      GetAccessNodeRequests(ServerConfig::GetInstance().request_arbor_urls,
                            kProbeDataEndpoint, params);

  rapidjson::Document cell_infos_json;
  cell_infos_json.SetArray();

  SPDLOG_TRACE("Parsing {} responses", probe_data_responses.size());
  for (const auto& cell_info : probe_data_responses) {
    rapidjson::Document info;
    info.Parse(cell_info.text.c_str(), cell_info.text.size());

    SPDLOG_TRACE("Received data from one simulator node: {}", cell_info.text);
    for (auto& entry : info["probeData"].GetArray()) {
      // cell_infos_json.PushBack(
      //     rapidjson::GenericValue(entry, cell_infos_json.GetAllocator()),
      //     cell_infos_json.GetAllocator());
      rapidjson::Value insert(entry, json_alloc);
      cell_infos_json.PushBack(insert, json_alloc);
    }
  }

  rapidjson::Value probe_data_response;
  probe_data_response.SetObject();
  probe_data_response.AddMember("probeData", cell_infos_json, json_alloc);
  probe_data_response.Accept(writer);
  SPDLOG_TRACE("Resulting JSON: {}", buffer.GetString());

  return {std::string(buffer.GetString())};
}
}  // namespace insite::arbor
