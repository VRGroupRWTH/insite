#include <config.h>

#include <nest/spikes.h>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include "query_params.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tl/optional.hpp"

namespace insite {
void CheckSpikeDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value>& spike_object) {
  assert(spike_object.HasMember(json_strings::kSimTimes) &&
         spike_object[json_strings::kSimTimes].IsArray());
  assert(spike_object.HasMember(json_strings::kNodeIds) &&
         spike_object[json_strings::kNodeIds].IsArray());
  assert(spike_object.HasMember(json_strings::kLastFrame) &&
         spike_object[json_strings::kLastFrame].IsBool());
}

SpikeVector ConvertFromJsonToSpikes(
    const rapidjson::GenericObject<false, rapidjson::Value>& data) {
  CheckSpikeDataValid(data);

  SpikeVector res;

  for (int i = 0; i < data[json_strings::kSimTimes].GetArray().Size(); ++i) {
    Spike spike{data[json_strings::kSimTimes].GetArray()[i].GetDouble(),
                data[json_strings::kNodeIds].GetArray()[i].GetUint64()};
    res.emplace_back(spike);
  }

  return res;
}

SpikeContainer NestGetSpikesFB(const SpikeParameter& parameter) {
  spdlog::stopwatch stopwatch;
  SPDLOG_DEBUG("Getting spikes from nodes...");
  std::string query_string =
      BuildQueryString("/spikesfb", parameter.GetParameterVector());

  auto spike_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, query_string);
  SPDLOG_DEBUG("Got spikes in {}", stopwatch.elapsed());

  bool last_frame;
  SpikeContainer spikes;

  stopwatch.reset();
  for (const cpr::Response& spike_data_set : spike_data_sets) {
    spikes.AddSpikesFromFlatbuffer(spike_data_set.text.c_str());
  }

  return spikes;
}

crow::response NestGetSpikes(const SpikeParameter& parameter, int api_version) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  spdlog::stopwatch stopwatch;
  SpikeParameter params_without_skip = parameter;
  params_without_skip.skip = tl::nullopt;
  std::string query_string =
      BuildQueryString("/spikes", params_without_skip.GetParameterVector());

  auto spike_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, query_string, api_version);

  bool last_frame;
  SpikeContainer spikes;
  std::string sim_id;

  stopwatch.reset();

  for (const cpr::Response& spike_data_set : spike_data_sets) {
    rapidjson::Document document;
    document.Parse(spike_data_set.text.c_str());
    // spdlog::error(spike_data_set.text.c_str());

    if (api_version == 1) {
      spikes.AddSpikesFromJson(document);
    } else if (api_version == 2) {
      sim_id = document["simId"].GetString();
      spikes.AddSpikesFromJson(document);
    }
  }

  spdlog::stopwatch sw_serialize;
  if (!parameter.sort || parameter.sort != false) {
    spikes.SortByTime();
  }

  if (api_version == 1) {
    writer.StartObject();
    spikes.SerializeToJson(writer, parameter.skip, parameter.top,
                           parameter.reverse_order, true);
    writer.EndObject();
  }

  if (api_version == 2) {
    spikes.SerializeToJsonV2(writer, parameter.skip, parameter.top,
                             parameter.reverse_order, true, sim_id);
  }
  SPDLOG_TRACE("Serialized in {}", sw_serialize.elapsed());
  return {buffer.GetString()};
}

//
// #################### ENDPOINT DEFINITIONS ####################
//
crow::response Spikesfb(const crow::request& req) {
  SpikeParameter params(req.url_params);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  flatbuffers::FlatBufferBuilder builder;

  SpikeContainer spikes = std::move(NestGetSpikesFB(params));
  spdlog::stopwatch sw_serialize;
  spikes.SerializeToJson(writer, spikes.Begin(params.skip, params.top), true);
  SPDLOG_INFO("Serialized in {}", sw_serialize.elapsed());

  return {buffer.GetString()};
}

}  // namespace insite
