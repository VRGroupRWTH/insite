#include "cpr/response.h"
#include "crow/http_response.h"
#include "jsonStrings.h"
#include "utilityFunctions.h"
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <nest/nestMultimeter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace insite {

// Receives a rapidjson-Document and checks if it has all the necessary
// properties for multimeter measurements
void CheckMultimeterMeasurementValid(
    const rapidjson::GenericObject<false, rapidjson::Value>& multimeter) {
  assert(multimeter.HasMember(json_strings::kSimTimes) &&
         multimeter[json_strings::kSimTimes].IsArray());
  assert(multimeter.HasMember(json_strings::kNodeIds) &&
         multimeter[json_strings::kNodeIds].IsArray());
  assert(multimeter.HasMember(json_strings::kValues) &&
         multimeter[json_strings::kValues].IsArray());
}

// Receives a rapidjson-Object and checks if it has all the necessary properties
// for multimeterData
void CheckMultimeterDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value>& multimeter) {
  assert(multimeter.HasMember(json_strings::kMultimeterId) &&
         multimeter[json_strings::kMultimeterId].IsInt());
  assert(multimeter.HasMember(json_strings::kNodeIds) &&
         multimeter[json_strings::kNodeIds].IsArray());
  assert(multimeter.HasMember(json_strings::kAttributes) &&
         multimeter[json_strings::kNodeIds].IsArray());
}

// Get all nest multimeters from the nest-server
std::unordered_map<uint64_t, MultimeterContainer> NestGetMultimeters() {
  // get request results and store in array
  rapidjson::MemoryPoolAllocator<> json_alloc;
  CprResponseVec multimeter_responses = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, "/multimeters");

  std::unordered_map<uint64_t, MultimeterContainer> merged_multimeters;

  for (auto& multimeter : multimeter_responses) {
    rapidjson::Document multimeter_resp_doc;
    multimeter_resp_doc.Parse(multimeter.text.c_str());

    for (auto& multimeter : multimeter_resp_doc.GetArray()) {
      auto multimeter_id = multimeter[json_strings::kMultimeterId].GetUint64();
      auto& multimeter_container = merged_multimeters[multimeter_id];
      multimeter_container.AddDataFromJson(multimeter);
    }
  }

  return merged_multimeters;
}

crow::response MultimeterResponse() {
  auto multimeters = std::move(NestGetMultimeters());

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartArray();
  for (const auto& multimeter : multimeters) {
    multimeter.second.SerializeToJson(writer);
  }
  writer.EndArray();

  return {buffer.GetString()};
}

crow::response MultimeterByIdResponse(uint64_t requested_multimeter_id) {
  auto multimeter = std::move(NestGetMultimeterById(requested_multimeter_id));

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  multimeter.SerializeToJson(writer);

  return {buffer.GetString()};
}

MultimeterContainer NestGetMultimeterById(uint64_t requested_multimeter_id) {
  CprResponseVec multimeter_responses = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, "/multimeters");

  MultimeterContainer result_multimeter;

  for (auto& multimeter : multimeter_responses) {
    rapidjson::Document multimeter_resp_doc;
    multimeter_resp_doc.Parse(multimeter.text.c_str());

    for (auto& multimeter : multimeter_resp_doc.GetArray()) {
      auto multimeter_id = multimeter[json_strings::kMultimeterId].GetUint64();
      if (multimeter_id != requested_multimeter_id) {
        continue;
      }
      result_multimeter.AddDataFromJson(multimeter);
    }
  }

  return result_multimeter;
}

MultimeterValueContainer NestGetMultimeterAttributes(
    const uint64_t multimeter_id,
    const std::string& attribute_name,
    MultimeterParameters parameters) {
  MultimeterContainer multimeter = NestGetMultimeterById(multimeter_id);

  std::vector<OptionalParameter> passthrough_params;
  passthrough_params.push_back({json_strings::kAttribute, attribute_name});
  passthrough_params.push_back({json_strings::kMultimeterId, multimeter_id});
  passthrough_params.emplace_back(parameters.GetValue(json_strings::kFromTime));
  passthrough_params.emplace_back(parameters.GetValue(json_strings::kToTime));
  passthrough_params.emplace_back(parameters.GetValue(json_strings::kNodeIds));

  CprResponseVec responses =
      GetAccessNodeRequests("/multimeter_measurement", passthrough_params);

  std::vector<uint64_t> node_id_vec;
  if (!parameters.GetValue(json_strings::kNodeIds)) {
    std::copy(multimeter.node_ids_.begin(), multimeter.node_ids_.end(),
              std::inserter(node_id_vec, node_id_vec.end()));
  } else {
    node_id_vec = multimeter.node_ids_;
  }

  MultimeterValueContainer multimeter_values(node_id_vec);
  for (auto& response : responses) {
    rapidjson::Document json_doc;
    json_doc.Parse(response.text.c_str());
    multimeter_values.AddDataFromJson(json_doc);
  }

  // TODO: Think about how we handle top and skip in this case here

  return multimeter_values;
}

crow::response MultimeterAttributesResponse(const crow::request& req,
                                            int multimeter_id,
                                            const std::string& attr_name) {
  MultimeterParameters parameters(req);
  // create empty rapidjson-document to store later results
  MultimeterValueContainer result = std::move(
      NestGetMultimeterAttributes(multimeter_id, attr_name, parameters));

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  result.SerializeToJson(writer);

  return {buffer.GetString()};
}

}  // namespace insite
