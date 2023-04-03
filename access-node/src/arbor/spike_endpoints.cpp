#include <insite_defines.h>

#include <arbor/arbor_strings.h>
#include <arbor/cell_endpoints.h>
#include <arbor/spike_endpoints.h>
#include <config.h>
#include <utility_functions.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "spdlog/spdlog.h"

namespace insite::arbor {

crow::response GetSpikes(const crow::request& req) {
  SPDLOG_DEBUG("Called Arbor GetCellInfos");
  rapidjson::StringBuffer buffer;
  RapidjsonWriter writer(buffer);

  auto params = GetQueryString(req.raw_url);

  auto cell_info_responses = QueryRandomNode(
      ServerConfig::GetInstance().request_arbor_urls, kSpikeEndpoint, params);

  return cell_info_responses.text;
}
}  // namespace insite::arbor
