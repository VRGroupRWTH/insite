#pragma once
#include <config.h>
#include <cpr/cpr.h>
#include <nest/json_strings.h>
#include <nest/spikerecorder_container.h>
#include <nest/spikes.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <utility_functions.h>
#include <iostream>
#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>

namespace insite {
// Receives a rapidjson-Object and checks if it has all the necessary properties
// for spikeRecorderData
void CheckSpikeRecorderDataValid(const rapidjson::GenericObject<false, rapidjson::Value>& json_spikerecorder);

std::vector<SpikerecorderContainer>
GetSpikerecorder(const rapidjson::Value& json_spikerecorders);

//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response SpikeRecorders(int api_version);

crow::response SpikesBySpikeRecorderId(const crow::request& req, int api_version, int requested_spikerecorder_id);

}  // namespace insite
