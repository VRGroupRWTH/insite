#pragma once
#include "query_params.h"
#include <iostream>
#include <iterator>
#include <nest/spike_container.h>
#include <optional>
#include <pdqsort.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <schema_generated.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <vector>
// #include "pdqsort.h"
#include <utility_functions.h>

namespace insite {
// Receives a rapidjson-Object and checks if it has all the necessary properties
// for spikeData
void CheckSpikeDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value> &spike_object);

// Convert the given rapidjson-Object to a spikeVector
SpikeVector ConvertFromJsonToSpikes(
    const rapidjson::GenericObject<false, rapidjson::Value> &data);
// Get all spikes from the nest-server by using the optional parameters
// fromTime, toTime, nodeIds, skip, top and sort

// SpikeContainer NestGetSpikesFB(const std::optional<double>& from_time,
//                                const std::optional<double>& to_time,
//                                const std::unordered_set<uint64_t>& node_ids,
//                                const std::optional<uint64_t>& skip,
//                                const std::optional<uint64_t>& top,
//                                const std::optional<std::string>& sort);

SpikeContainer NestGetSpikesFB(const SpikeParameter &parameter);
// SpikeContainer NestGetSpikes(const SpikeParameter &parameter, int
// api_version);
crow::response NestGetSpikes(const SpikeParameter &parameter, int api_version);
// crow::response Spikes(const crow::request &req);
// crow::response Spikes(const crow::request &req, int api_version);
crow::response Spikesfb(const crow::request &req);
} // namespace insite
