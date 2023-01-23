#pragma once
#include <cpr/cpr.h>
#include <crow.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <iostream>
#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>
#include "cpr/response.h"
#include <params.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <jsonParameters.h>
#include <jsonStrings.h>
#include <queryStringBuilder.h>
#include <spike.h>
namespace insite {

// converts a cpr-response into a string
std::string ParseResponseToString(const cpr::Response& response);

SpikeContainer ParseResponseToString2(const cpr::Response& response);

using CprResponseVec = std::vector<cpr::Response>;

void SpikesToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer,
                  std::vector<Spike>& spikes);
// Converts a rapidjson-document to a json-String
std::string DocumentToString(const rapidjson::Document& document);

std::string QueryRandomNode(const std::vector<std::string>& urls,
                            const std::string& postfix);
// create and allocate storage for http-response
std::vector<std::future<std::string>> GetAccessNodeRequests(
    const std::vector<std::string>& urls,
    const std::string& postfix);
// Send and return the requests to the accessNode you get when combining all the
// given urls with the given postfix
CprResponseVec GetAccessNodeRequests2(const std::vector<std::string>& urls,
                                      const std::string& postfix);

CprResponseVec GetAccessNodeRequests2(const std::string& query_string);

CprResponseVec GetAccessNodeRequests2(
    const std::string& endpoint,
    const std::vector<OptionalParameter>& params);

CprResponseVec NestNodeRequests(const std::string& endpoint);
}  // namespace insite
