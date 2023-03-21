#pragma once
#include <cpr/cpr.h>
#include <crow.h>
// #include <params.h>
#include "cpr/response.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <iterator>
#include <optional>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <unordered_set>
#include <vector>

#include <jsonStrings.h>
#include <queryStringBuilder.h>
#include <spike.h>
namespace insite {

#ifdef INSITE_PRETTY_JSON
using RapidjsonWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
#else
using RapidjsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;
#endif

using CprResponseVec = std::vector<cpr::Response>;

std::string GetQueryString(const std::string &raw_url);

void SpikesToJson(rapidjson::Writer<rapidjson::StringBuffer> &writer,
                  std::vector<Spike> &spikes);
// Converts a rapidjson-document to a json-String
std::string DocumentToString(const rapidjson::Document &document);

cpr::Response QueryRandomNode(const std::vector<std::string> &urls,
                              const std::string &postfix, int api_version = 1);

cpr::Response QueryRandomNode(const std::vector<std::string> &urls,
                              const std::string &postfix,
                              const std::string &query_string,
                              int api_version = 1);

// create and allocate storage for http-response
// Send and return the requests to the accessNode you get when combining all the
// given urls with the given postfix
CprResponseVec GetAccessNodeRequests(const std::vector<std::string> &urls,
                                     const std::string &postfix,
                                     int api_version = 1);

CprResponseVec GetAccessNodeRequests(const std::vector<std::string> &urls,
                                     const std::string &postfix,
                                     const std::string &query_string,
                                     int api_version = 1);

// CprResponseVec GetAccessNodeRequests(
//     const std::string& endpoint,
//     const std::vector<OptionalParameter>& params);

} // namespace insite
