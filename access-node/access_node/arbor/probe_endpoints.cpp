#include <insite_defines.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "spdlog/spdlog.h"
#include "utilityFunctions.h"
#include <arbor/arbor_strings.h>
#include <arbor/probe_endpoints.h>
#include <config.h>

namespace insite::arbor {

crow::response GetProbes(const crow::request &req) {
  SPDLOG_DEBUG("Called Arbor GetProbes");
  rapidjson::StringBuffer buffer;
  RapidjsonWriter writer(buffer);

  auto params = GetQueryString(req.raw_url);
  auto probe_data_responses = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_arbor_urls, kProbeEndpoint, params);

  rapidjson::Document cell_infos_json;
  cell_infos_json.SetArray();

  SPDLOG_TRACE("Parsing {} responses", probe_data_responses.size());
  for (const auto &cell_info : probe_data_responses) {
    rapidjson::Document info;
    info.Parse(cell_info.text.c_str(), cell_info.text.size());

    SPDLOG_TRACE("Received data from one simulator node: {}", cell_info.text);
    for (auto &entry : info.GetArray()) {
      cell_infos_json.PushBack(
          rapidjson::GenericValue(entry, cell_infos_json.GetAllocator()),
          cell_infos_json.GetAllocator());
    }
  }

  cell_infos_json.Accept(writer);
  SPDLOG_TRACE("Resulting JSON: {}", buffer.GetString());

  return {std::string(buffer.GetString())};
}

crow::response GetProbeData(const crow::request &req) {
  SPDLOG_DEBUG("Called Arbor GetProbeData");
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
  for (const auto &cell_info : probe_data_responses) {
    rapidjson::Document info;
    info.Parse(cell_info.text.c_str(), cell_info.text.size());

    SPDLOG_TRACE("Received data from one simulator node: {}", cell_info.text);
    for (auto &entry : info.GetArray()) {
      cell_infos_json.PushBack(
          rapidjson::GenericValue(entry, cell_infos_json.GetAllocator()),
          cell_infos_json.GetAllocator());
    }
  }

  cell_infos_json.Accept(writer);
  SPDLOG_TRACE("Resulting JSON: {}", buffer.GetString());

  return {std::string(buffer.GetString())};
}
} // namespace insite::arbor
