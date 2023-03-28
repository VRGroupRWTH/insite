#include <insite_defines.h>

#include "crow/http_response.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "spdlog/spdlog.h"
#include <arbor/arbor_strings.h>
#include <arbor/cell_endpoints.h>
#include <config.h>
#include <utility_functions.h>

namespace insite::arbor {

crow::response GetCells(const crow::request &req) {

  SPDLOG_DEBUG("Called Arbor GetCellInfos");
  rapidjson::StringBuffer buffer;
  RapidjsonWriter writer(buffer);

  SPDLOG_DEBUG("Querying {} simulator instances",
               ServerConfig::GetInstance().request_arbor_urls.size());

  auto params = GetQueryString(req.raw_url);

  auto cell_info_responses = QueryRandomNode(
      ServerConfig::GetInstance().request_arbor_urls, kCellEndpoint, params);

  return cell_info_responses.text;
}

crow::response GetCellInfos(const crow::request &req) {

  SPDLOG_DEBUG("Called Arbor GetCellInfos");
  rapidjson::StringBuffer buffer;
  RapidjsonWriter writer(buffer);

  SPDLOG_DEBUG("Querying {} simulator instances",
               ServerConfig::GetInstance().request_arbor_urls.size());

  auto params = GetQueryString(req.raw_url);

  auto cell_info_responses =
      QueryRandomNode(ServerConfig::GetInstance().request_arbor_urls,
                      kCellInfoEndpoint, params);

  return cell_info_responses.text;
}
} // namespace insite::arbor
