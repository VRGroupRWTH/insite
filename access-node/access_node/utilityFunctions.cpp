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
#include "params.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "config.h"
#include "jsonParameters.h"
#include "jsonStrings.h"
#include "queryStringBuilder.h"
#include "spike.h"
#include "utilityFunctions.h"

namespace insite {
// converts a cpr-response into a string
std::string ParseResponseToString(const cpr::Response& response) {
  return response.text;
}

SpikeContainer ParseResponseToString2(const cpr::Response& response) {
  SpikeContainer spikes;
  spikes.AddSpikesFromJson(response.text.c_str());
  return spikes;
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
    writer.Double(spike.node_id);
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

std::string QueryRandomNode(const std::vector<std::string>& urls,
                            const std::string& postfix) {
  // create and allocate storage for http-responses
  std::vector<std::string>::const_iterator rand_it = urls.begin();
  std::advance(rand_it, std::rand() % urls.size());
  // Loop through all generic URLs, attach kernelStatus postfix and send async
  // request
  std::string url = *rand_it + postfix;
  // spdlog::debug("REQUEST: {}", url);
  auto response = cpr::GetAsync(cpr::Url(url));

  response.wait();
  return response.get().text;
}

// Send and return the requests to the accessNode you get when combining all the
// given urls with the given postfix
CprResponseVec GetAccessNodeRequests(const std::vector<std::string>& urls,
                                     const std::string& postfix) {
  // create and allocate storage for http-responses

  std::vector<std::shared_ptr<cpr::Session>> sessions;
  // Create MultiPerform object
  cpr::MultiPerform multiperform;
  for (const auto& url : urls) {
    auto session = std::make_shared<cpr::Session>();
    session->SetUrl(url + postfix);
    sessions.emplace_back(std::move(session));
    multiperform.AddSession(sessions.back());
    spdlog::debug(
        "[GetAccessNodeRequests2] Added {} to the multiperform request.",
        url + postfix);
  }
  // Add sessions to the MultiPerform
  return multiperform.Get();
}

CprResponseVec GetAccessNodeRequests(
    const std::string& endpoint,
    const std::vector<OptionalParameter>& params) {
  spdlog::debug("params: {}\n", OptionalParameters::ToQueryString(params));
  std::string query_string =
      endpoint + OptionalParameters::ToQueryString(params);
  return GetAccessNodeRequests(ServerConfig::GetInstance().request_urls,
                               query_string);
}

CprResponseVec NestNodeRequests(const std::string& endpoint) {
  std::vector<std::shared_ptr<cpr::Session>> sessions;
  // Create MultiPerform object
  cpr::MultiPerform multiperform;
  auto urls = ServerConfig::GetInstance().request_urls;
  for (const auto& url : urls) {
    auto session = std::make_shared<cpr::Session>();
    session->SetUrl(url + endpoint);
    sessions.emplace_back(std::move(session));
    multiperform.AddSession(sessions.back());
    spdlog::debug(
        "[GetAccessNodeRequests2] Added {} to the multiperform request.",
        url + endpoint);
  }
  // Add sessions to the MultiPerform
  return multiperform.Get();
}
}  // namespace insite
