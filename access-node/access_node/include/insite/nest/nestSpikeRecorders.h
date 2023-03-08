#pragma once
#include <config.h>
#include <cpr/cpr.h>
#include <jsonStrings.h>
#include <nest/nestSpikes.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <spikedetector.h>
#include <utilityFunctions.h>
#include <iostream>
#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>
#include "query_params.h"

namespace insite {
// Receives a rapidjson-Object and checks if it has all the necessary properties
// for spikeRecorderData
void CheckSpikeRecorderDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value>&
        json_spikerecorder) {
  assert(json_spikerecorder.HasMember(json_strings::kSpikerecorderId) &&
         json_spikerecorder[json_strings::kSpikerecorderId].IsInt());
  assert(json_spikerecorder.HasMember(json_strings::kNodeIds) &&
         json_spikerecorder[json_strings::kNodeId].IsArray());
}

std::vector<Spikerecorder> GetSpikerecorder() {
  std::vector<Spikerecorder> spikerecorders;
  std::string response = QueryRandomNode(
      ServerConfig::GetInstance().request_urls, "/spikerecorders");
  rapidjson::Document json_spikerecorders;
  json_spikerecorders.Parse(response.c_str());

  for (const auto& recorder : json_spikerecorders.GetArray()) {
    spikerecorders.emplace_back(recorder);
  }

  return spikerecorders;
}

//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response SpikeRecorders() {
  rapidjson::StringBuffer json_buffer;
  rapidjson::Writer<rapidjson::StringBuffer> json_writer(json_buffer);

  rapidjson::MemoryPoolAllocator<> json_alloc;
  json_writer.StartArray();
  for (const auto& spikerecorder : GetSpikerecorder()) {
    spikerecorder.WriteToJson(json_writer);
  }
  json_writer.EndArray();

  return {json_buffer.GetString()};
}

crow::response SpikesBySpikeRecorderId(const crow::request& req,
                                       int requested_spikerecorder_id) {
  SpikeParameter params(req.url_params);

  std::vector<Spikerecorder> spikerecorders = GetSpikerecorder();
  const auto& spikerecorder = std::find_if(
      spikerecorders.begin(), spikerecorders.end(),
      [&](const Spikerecorder& spikerecorder) {
        return spikerecorder.spikerecorder_id == requested_spikerecorder_id;
      });

  if (spikerecorder == spikerecorders.end()) {
    return {"Error"};
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  std::vector<uint64_t> query_node_ids;
  if (!params.node_gids) {
    params.node_gids = spikerecorder->nodes;
    SpikeContainer spikes = std::move(NestGetSpikes(params));
    spikes.SerializeToJson(writer, spikes.Begin(params.skip, params.top), true);
    if (!params.sort || params.sort != false) {
      spikes.SortByTime();
    }
    spikes.SerializeToJson(writer, params.skip, params.top,
                           params.reverse_order, true);

    return {buffer.GetString()};
  }

  for (const auto nodes : spikerecorder->nodes) {
    if (std::find(params.node_gids.value().begin(),
                  params.node_gids.value().end(),
                  nodes) != params.node_gids.value().end()) {
      query_node_ids.push_back(nodes);
    }
  }

  if (query_node_ids.empty()) {
    SpikeContainer empty{};
    empty.SerializeToJson(writer, false);
    return {buffer.GetString()};
  }

  SpikeContainer spikes = std::move(NestGetSpikes(params));
  spikes.SerializeToJson(writer, params.skip, params.top, params.reverse_order,
                         true);
  return {buffer.GetString()};
}
}  // namespace insite
