#include "config.h"
#include "cpr/response.h"
#include "crow/http_response.h"
#include "query_params.h"
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <nest/json_strings.h>
#include <nest/multimeter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <unordered_map>
#include <utility>
#include <utility_functions.h>
#include <vector>

namespace insite {

// Receives a rapidjson-Document and checks if it has all the necessary
// properties for multimeter measurements
void CheckMultimeterMeasurementValid(
    const rapidjson::GenericObject<false, rapidjson::Value> &multimeter) {
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
    const rapidjson::GenericObject<false, rapidjson::Value> &multimeter) {
  assert(multimeter.HasMember(json_strings::kMultimeterId) &&
         multimeter[json_strings::kMultimeterId].IsInt());
  assert(multimeter.HasMember(json_strings::kNodeIds) &&
         multimeter[json_strings::kNodeIds].IsArray());
  assert(multimeter.HasMember(json_strings::kAttributes) &&
         multimeter[json_strings::kNodeIds].IsArray());
}

crow::response MultimeterResponse(int api_version) {
  CprResponseVec multimeter_responses =
      GetAccessNodeRequests(ServerConfig::GetInstance().request_nest_urls,
                            "/multimeters", api_version);

  std::unordered_map<uint64_t, MultimeterContainer> merged_multimeters;
  std::string sim_id;

  for (auto &multimeter : multimeter_responses) {
    rapidjson::Document multimeter_resp_doc;
    multimeter_resp_doc.Parse(multimeter.text.c_str());

    if (api_version == 1) {
      for (auto &multimeter : multimeter_resp_doc.GetArray()) {
        auto multimeter_id =
            multimeter[json_strings::kMultimeterId].GetUint64();
        auto &multimeter_container = merged_multimeters[multimeter_id];
        multimeter_container.AddDataFromJson(multimeter);
      }
    }

    if (api_version == 2) {
      sim_id = multimeter_resp_doc["simId"].GetString();
      for (auto &multimeter : multimeter_resp_doc["multimeter"].GetArray()) {
        auto multimeter_id =
            multimeter[json_strings::kMultimeterId].GetUint64();
        auto &multimeter_container = merged_multimeters[multimeter_id];
        multimeter_container.AddDataFromJson(multimeter);
      }
    }
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  if (api_version == 1) {
    writer.StartArray();
    for (const auto &multimeter : merged_multimeters) {
      multimeter.second.SerializeToJson(writer);
    }
    writer.EndArray();
  } else if (api_version == 2) {
    writer.StartObject();
    writer.Key("simId");
    writer.String(sim_id.c_str());
    writer.Key("multimeter");
    writer.StartArray();
    for (const auto &multimeter : merged_multimeters) {
      multimeter.second.SerializeToJson(writer);
    }
    writer.EndArray();
    writer.EndObject();
  }

  return {buffer.GetString()};
}

crow::response MultimeterByIdResponse(int api_version,
                                      uint64_t requested_multimeter_id) {
  CprResponseVec multimeter_responses =
      GetAccessNodeRequests(ServerConfig::GetInstance().request_nest_urls,
                            "/multimeters", api_version);

  std::unordered_map<uint64_t, MultimeterContainer> merged_multimeters;
  std::string sim_id;

  for (auto &multimeter : multimeter_responses) {
    rapidjson::Document multimeter_resp_doc;
    multimeter_resp_doc.Parse(multimeter.text.c_str());

    if (api_version == 1) {
      for (auto &multimeter : multimeter_resp_doc.GetArray()) {
        auto multimeter_id =
            multimeter[json_strings::kMultimeterId].GetUint64();
        auto &multimeter_container = merged_multimeters[multimeter_id];
        multimeter_container.AddDataFromJson(multimeter);
      }
    }

    if (api_version == 2) {
      sim_id = multimeter_resp_doc["simId"].GetString();
      for (auto &multimeter : multimeter_resp_doc["multimeter"].GetArray()) {
        auto multimeter_id =
            multimeter[json_strings::kMultimeterId].GetUint64();
        auto &multimeter_container = merged_multimeters[multimeter_id];
        multimeter_container.AddDataFromJson(multimeter);
      }
    }
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  if (api_version == 1) {
    merged_multimeters[requested_multimeter_id].SerializeToJson(writer);
  } else if (api_version == 2) {
    writer.StartObject();
    writer.Key("simId");
    writer.String(sim_id.c_str());
    writer.Key("multimeter");
    merged_multimeters[requested_multimeter_id].SerializeToJson(writer);
    writer.EndObject();
  }

  return {buffer.GetString()};
}

MultimeterContainer NestGetMultimeterById(uint64_t requested_multimeter_id) {
  CprResponseVec multimeter_responses = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/multimeters");

  MultimeterContainer result_multimeter;

  for (auto &multimeter : multimeter_responses) {
    rapidjson::Document multimeter_resp_doc;
    multimeter_resp_doc.Parse(multimeter.text.c_str());

    for (auto &multimeter : multimeter_resp_doc.GetArray()) {
      auto multimeter_id = multimeter[json_strings::kMultimeterId].GetUint64();
      if (multimeter_id != requested_multimeter_id) {
        continue;
      }
      result_multimeter.AddDataFromJson(multimeter);
    }
  }

  return result_multimeter;
}

MultimeterValueContainer
NestGetMultimeterAttributesV2(const uint64_t multimeter_id,
                              const std::string &attribute_name,
                              const MultimeterParameter &parameters) {
  MultimeterContainer multimeter = NestGetMultimeterById(multimeter_id);

  std::string query_string = BuildQueryString("/multimeter_measurement",
                                              parameters.GetParameterVector());

  auto mm_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, query_string, 2);

  std::vector<uint64_t> node_id_vec;
  if (parameters.node_gids.empty()) {
    std::copy(multimeter.node_ids_.begin(), multimeter.node_ids_.end(),
              std::inserter(node_id_vec, node_id_vec.end()));
  } else {
    node_id_vec = multimeter.node_ids_;
  }

  MultimeterValueContainer multimeter_values(node_id_vec);
  for (auto &response : mm_data_sets) {
    rapidjson::Document json_doc;
    json_doc.Parse(response.text.c_str());
    multimeter_values.AddDataFromJson(json_doc);
    multimeter_values.sim_id = json_doc["simId"];
  }

  // TODO: Think about how we handle top and skip in this case here

  return multimeter_values;
}

MultimeterValueContainer
NestGetMultimeterAttributes(const uint64_t multimeter_id,
                            const std::string &attribute_name,
                            const MultimeterParameter &parameters) {
  MultimeterContainer multimeter = NestGetMultimeterById(multimeter_id);

  std::string query_string = BuildQueryString("/multimeter_measurement",
                                              parameters.GetParameterVector());

  auto mm_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, query_string);

  std::vector<uint64_t> node_id_vec;
  if (parameters.node_gids.empty()) {
    std::copy(multimeter.node_ids_.begin(), multimeter.node_ids_.end(),
              std::inserter(node_id_vec, node_id_vec.end()));
  } else {
    node_id_vec = multimeter.node_ids_;
  }

  MultimeterValueContainer multimeter_values(node_id_vec);
  for (auto &response : mm_data_sets) {
    rapidjson::Document json_doc;
    json_doc.Parse(response.text.c_str());
    multimeter_values.AddDataFromJson(json_doc);
  }

  return multimeter_values;
}

crow::response MultimeterAttributesResponse(const crow::request &req,
                                            int api_version, int multimeter_id,
                                            const std::string &attr_name) {
  MultimeterParameter params(req.url_params);
  params.multimeter_id = multimeter_id;
  params.attribute = attr_name;

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  if (api_version == 1) {
    MultimeterValueContainer result = std::move(
        NestGetMultimeterAttributes(multimeter_id, attr_name, params));
    result.SerializeToJson(writer);

  } else if (api_version == 2) {
    MultimeterValueContainer result = std::move(
        NestGetMultimeterAttributesV2(multimeter_id, attr_name, params));
    result.SerializeToJsonV2(writer);
  }
  return {buffer.GetString()};
}

} // namespace insite
