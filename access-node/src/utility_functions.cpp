#include "config.h"

#include <cpr/async.h>
#include <cpr/cpr.h>
#include <crow.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>
#include "cpr/session.h"
// #include "params.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

// #include "jsonParameters.h"
#include <nest/json_strings.h>
#include <nest/spike_container.h>
#include <query_string_builder.h>
#include <utility_functions.h>

namespace insite {

std::string GetQueryString(const std::string& raw_url) {
  auto query_string_start = raw_url.find_first_of('?');

  return query_string_start != std::string::npos
             ? raw_url.substr(query_string_start)
             : "";
}

// Takes a reference to a rapidjson writer and a reference to a vector of Spikes
// (node_id,time pairs) and writes the spikes into the JSON writer
//  jsonStrings::simTimes: [0.0,0.1,...], jsonStrings::nodeIds:[3,6,...]
void SpikesToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer,
                  std::vector<Spike>& spikes) {
  writer.StartObject();

  writer.Key(json_strings::kSimTimes);
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Double(spike.time);
  }
  writer.EndArray();

  writer.Key(json_strings::kNodeIds);
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Int64(spike.node_id);
  }
  writer.EndArray();

  writer.EndObject();
}

// Converts a rapidjson-document to a json-String
std::string DocumentToString(const rapidjson::Document& document) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);

  return {buffer.GetString()};
}

cpr::Response QueryRandomNode(const std::vector<std::string>& urls,
                              const std::string& postfix,
                              int api_version) {
  // create and allocate storage for http-responses
  auto rand_it = urls.begin();
  std::advance(rand_it, std::rand() % urls.size());
  // Loop through all generic URLs, attach kernelStatus postfix and send async
  std::string version =
      api_version > 1 ? "/v" + std::to_string(api_version) : "";

  std::string url = *rand_it + version + postfix;

  SPDLOG_TRACE("Querying simulator instance: {}", url);
  auto response = cpr::GetAsync(cpr::Url(url));

  response.wait();
  return response.get();
}

cpr::Response QueryRandomNode(const std::vector<std::string>& urls,
                              const std::string& postfix,
                              const std::string& query_string,
                              int api_version) {
  return QueryRandomNode(urls, postfix + query_string, api_version);
}

// Send and return the requests to the accessNode you get when combining all the
// given urls with the given postfix
CprResponseVec GetAccessNodeRequests(const std::vector<std::string>& urls,
                                     const std::string& postfix,
                                     int api_version) {
  // create and allocate storage for http-responses

  std::string versioned_postfix =
      api_version > 1 ? "/v" + std::to_string(api_version) : "";
  versioned_postfix += postfix;

  std::vector<std::shared_ptr<cpr::Session>> sessions;
  // Create MultiPerform object
  cpr::MultiPerform multiperform;
  for (const auto& url : urls) {
    auto session = std::make_shared<cpr::Session>();
    session->SetUrl(url + versioned_postfix);
    sessions.emplace_back(std::move(session));
    multiperform.AddSession(sessions.back());
    SPDLOG_TRACE(
        "[GetAccessNodeRequests] Added {} to the multiperform request.",
        url + versioned_postfix);
  }
  // Add sessions to the MultiPerform
  return multiperform.Get();
}

CprResponseVec GetAccessNodeRequests(const std::vector<std::string>& urls,
                                     const std::string& postfix,
                                     const std::string& query_string,
                                     int api_version) {
  return GetAccessNodeRequests(urls, postfix + query_string, api_version);
}

// CprResponseVec GetAccessNodeRequests(
//     const std::string& endpoint,
//     const std::vector<OptionalParameter>& params) {
//   SPDLOG_DEBUG("params: {}\n", OptionalParameters::ToQueryString(params));
//   std::string query_string =
//       endpoint + OptionalParameters::ToQueryString(params);
//   return GetAccessNodeRequests(ServerConfig::GetInstance().request_urls,
//                                query_string);
// }

}  // namespace insite
