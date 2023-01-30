#include <config.h>
#include <nest/nestSpikes.h>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include "spdlog/common.h"
#include "spdlog/spdlog.h"

namespace insite {
// Receives a rapidjson-Object and checks if it has all the necessary properties
// for spikeData
void CheckSpikeDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value>& spike_object) {
  assert(spike_object.HasMember(json_strings::kSimTimes) &&
         spike_object[json_strings::kSimTimes].IsArray());
  assert(spike_object.HasMember(json_strings::kNodeIds) &&
         spike_object[json_strings::kNodeIds].IsArray());
  assert(spike_object.HasMember(json_strings::kLastFrame) &&
         spike_object[json_strings::kLastFrame].IsBool());
}

// Convert the given rapidjson-Object to a spikeVector
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

SpikeContainer NestGetSpikesFB(const std::optional<double>& from_time,
                               const std::optional<double>& to_time,
                               const std::unordered_set<uint64_t>& node_ids,
                               const std::optional<uint64_t>& skip,
                               const std::optional<uint64_t>& top,
                               const std::optional<std::string>& sort) {
  std::vector<Parameter> params;

  if (from_time) {
    params.emplace_back(json_strings::kFromTime,
                        std::to_string(from_time.value()));
  }

  if (to_time) {
    params.emplace_back(json_strings::kToTime, std::to_string(to_time.value()));
  }

  if (!node_ids.empty()) {
    params.emplace_back(json_strings::kNodeIds, UnorderedSetToCsv(node_ids));
  }
  spdlog::stopwatch stopwatch;
  SPDLOG_DEBUG("Getting spikes from nodes...");
  std::string query_string = BuildQueryString("/spikesfb", params);

  auto spike_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, query_string);
  SPDLOG_DEBUG("Got spikes in {}", stopwatch.elapsed());

  bool last_frame;
  SpikeContainer spikes;
  SpikeContainer spikes2;

  stopwatch.reset();
  for (const cpr::Response& spike_data_set : spike_data_sets) {
    spikes.AddSpikesFromFlatbuffer(spike_data_set.text.c_str());
  }
  SPDLOG_DEBUG("Added spikes in {}", stopwatch.elapsed());

  if (sort && sort.value() == json_strings::kSimTimes) {
    spdlog::stopwatch stopwatch_sort;
    spikes.SortByTime();
    SPDLOG_DEBUG("Sorted spikes in {}", stopwatch_sort.elapsed());
  } else if (sort && sort.value() == json_strings::kNodeIds) {
    spdlog::stopwatch stopwatch_sort;
    spikes.SortByTimePdq();
    SPDLOG_DEBUG("Sorted spikes in {}", stopwatch_sort.elapsed());
    // spikes.sortByNodeId();
  }

  return spikes;
}
// Get all spikes from the nest-server by using the optional parameters
// fromTime, toTime, nodeIds, skip, top and sort
SpikeContainer NestGetSpikes(const std::optional<double>& from_time,
                             const std::optional<double>& to_time,
                             const std::unordered_set<uint64_t>& node_ids,
                             const std::optional<uint64_t>& skip,
                             const std::optional<uint64_t>& top,
                             const std::optional<std::string>& sort) {
  std::vector<Parameter> params;

  if (from_time) {
    params.emplace_back(json_strings::kFromTime,
                        std::to_string(from_time.value()));
  }

  if (to_time) {
    params.emplace_back(json_strings::kToTime, std::to_string(to_time.value()));
  }

  if (!node_ids.empty()) {
    params.emplace_back(json_strings::kNodeIds, UnorderedSetToCsv(node_ids));
  }
  spdlog::stopwatch stopwatch;
  SPDLOG_DEBUG("Getting spikes from nodes...");
  std::string query_string = BuildQueryString("/spikes", params);

  auto spike_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, query_string);
  SPDLOG_DEBUG("Got spikes in {}", stopwatch.elapsed());

  bool last_frame;
  SpikeContainer spikes;
  SpikeContainer spikes2;

  stopwatch.reset();
  for (const cpr::Response& spike_data_set : spike_data_sets) {
    // rapidjson::Document spike_data_new;
    // spike_data_new.Parse(spike_data_set.c_str());
    //
    // assert(spike_data_new.IsObject());
    // checkSpikeDataValid(spike_data_new.GetObject());
    //
    // lastFrame =
    // spike_data_new.GetObject()[json_strings::lastFrame].GetBool();

    spikes.AddSpikesFromJson(spike_data_set.text.c_str());
  }

  if (sort && sort.value() == json_strings::kSimTimes) {
    spdlog::stopwatch sw_sorting;
    spikes.SortByTime();
    SPDLOG_DEBUG("Sorted spikes in {}", stopwatch.elapsed());
  } else if (sort && sort.value() == json_strings::kNodeIds) {
    spdlog::stopwatch sw_sorting;
    spikes.SortByTimePdq();
    SPDLOG_DEBUG("Sorted spikes in {}", stopwatch.elapsed());
    // spikes.sortByNodeId();
  }

  return spikes;
}

SpikeContainer NestGetSpikes(const std::optional<double>& from_time,
                             const std::optional<double>& to_time,
                             const std::vector<uint64_t>& node_ids,
                             const std::optional<uint64_t>& skip,
                             const std::optional<uint64_t>& top,
                             const std::optional<std::string>& sort) {
  const std::unordered_set<uint64_t> node_ids_set(node_ids.begin(),
                                                  node_ids.end());
  return NestGetSpikes(from_time, to_time, node_ids_set, skip, top, sort);
}

SpikeContainer NestGetSpikes(const SpikeParameter& parameter) {
  std::optional<uint64_t> skip;
  std::optional<uint64_t> top;
  std::optional<std::string> sort;
  if (parameter.skip) {
    skip = parameter.skip.value();
  }

  if (parameter.top) {
    top = parameter.top.value();
  }

  if (parameter.sort) {
    top = parameter.sort.value();
  }

  return NestGetSpikes(
      parameter.from_time.value_or(0),
      parameter.to_time.value_or(std::numeric_limits<std::uint64_t>::max()),
      parameter.node_gids.value_or(std::vector<uint64_t>{}), skip, top, sort);
}
// Get all spikes from the nest-server by using the optional parameters
// fromTime, toTime, nodeIds, skip, top and sort

//
// #################### ENDPOINT DEFINITIONS ####################
//
crow::response Spikesfb(const crow::request& req) {
  std::optional<double> from_time =
      GetParam<double>(req, json_strings::kFromTime);
  std::optional<double> to_time = GetParam<double>(req, json_strings::kToTime);
  std::unordered_set<uint64_t> node_ids =
      GetParamUSet<uint64_t>(req, json_strings::kNodeIds);
  std::optional<uint64_t> skip = GetParam<uint64_t>(req, json_strings::kSkip);
  std::optional<uint64_t> top = GetParam<uint64_t>(req, json_strings::kTop);
  std::optional<std::string> sort =
      GetParam<std::string>(req, json_strings::kSort);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  flatbuffers::FlatBufferBuilder builder;

  SpikeContainer spikes =
      std::move(NestGetSpikesFB(from_time, to_time, node_ids, skip, top, sort));
  spdlog::stopwatch sw_serialize;
  spikes.SerializeToJson(writer, spikes.Begin(skip, top), true);
  SPDLOG_DEBUG("Serialized in {}", sw_serialize.elapsed());

  return {buffer.GetString()};
}

crow::response Spikes(const crow::request& req) {
  SpikeParameter params(req.url_params);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  SpikeContainer spikes = std::move(NestGetSpikes(params));
  spdlog::stopwatch sw_serialize;
  spikes.SerializeToJson(writer, spikes.Begin(params.skip, params.top), true);
  SPDLOG_DEBUG("Serialized in {}", sw_serialize.elapsed());

  return {buffer.GetString()};
}
}  // namespace insite
