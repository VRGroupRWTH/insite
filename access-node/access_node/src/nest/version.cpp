#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <config.h>
#include <fstream>
#include <iterator>
#include <nest/version.h>
#include <optional>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <utility_functions.h>
#include <vector>

namespace insite {
rapidjson::Value
NestGetSimulationTimeInfo(rapidjson::MemoryPoolAllocator<> json_alloc,
                          int api_version) {
  // get request results back and store in array
  auto sim_time_infos =
      GetAccessNodeRequests(ServerConfig::GetInstance().request_nest_urls,
                            "/simulationTimeInfo", api_version);

  double begin_time = 0;
  double current_time = std::numeric_limits<double>::max();
  double end_time = 0;
  double step_size = 0;
  rapidjson::Value sim_id;

  // loop through all kernelStatus-data-sets and create a new object for each
  for (const auto &sim_time_info : sim_time_infos) {
    std::string json_string = sim_time_info.text;

    // create rapidjson-document for current kernelStatus-data-set and parse
    // data into it
    rapidjson::Document current_sim_time_info;
    current_sim_time_info.Parse(json_string.c_str());

    assert(current_sim_time_info.IsObject());
    if (api_version == 2) {
      sim_id = current_sim_time_info.GetObject()[json_strings::kSimId];
    }

    begin_time = std::max(
        current_sim_time_info.GetObject()[json_strings::kBegin].GetDouble(),
        begin_time);
    current_time = std::min(
        current_sim_time_info.GetObject()[json_strings::kCurrent].GetDouble(),
        current_time);
    end_time = std::max(
        current_sim_time_info.GetObject()[json_strings::kEnd].GetDouble(),
        end_time);
    step_size =
        current_sim_time_info.GetObject()[json_strings::kStepSize].GetDouble();
  }

  rapidjson::Value result(rapidjson::kObjectType);

  if (api_version == 2) {
    result.AddMember(json_strings::kSimId, sim_id, json_alloc);
  }
  result.AddMember(json_strings::kBegin, begin_time, json_alloc);
  result.AddMember(json_strings::kCurrent, current_time, json_alloc);
  result.AddMember(json_strings::kEnd, end_time, json_alloc);
  result.AddMember(json_strings::kStepSize, step_size, json_alloc);

  return result;
}

rapidjson::Value NestGetVersion(rapidjson::MemoryPoolAllocator<> &json_alloc) {
  // get request results back and store in array
  auto nest_version_infos = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/version");

  rapidjson::Value api_version;
  rapidjson::Value insite_version;

  // loop through all kernelStatus-data-sets and create a new object for each
  for (auto &nest_version_info : nest_version_infos) {
    std::string json_string = nest_version_info.text;

    rapidjson::Document current_version_info;
    current_version_info.Parse(json_string.c_str());

    // assert(current_version_info.IsString());

    api_version.SetString(json_string.c_str(), json_alloc);
  }

  std::string insite_version_text;
  std::ifstream input;

  input.open("pathToFile");
  if (input) {
    std::getline(input, insite_version_text);
    input.close();

    rapidjson::Document insite_version_doc;
    insite_version_doc.Parse(insite_version_text.c_str());
    insite_version.SetString(insite_version_doc.GetString(), json_alloc);
  } else {
    insite_version.SetString("2.0_rc1", json_alloc);
  }

  rapidjson::Value result(rapidjson::kObjectType);
  result.GetObject().AddMember(json_strings::kApi, api_version, json_alloc);
  result.GetObject().AddMember(json_strings::kInsite, insite_version,
                               json_alloc);

  return result;
}

//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response SimulationTimeInfo(int api_version) {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value sim_time_info =
      NestGetSimulationTimeInfo(json_alloc, api_version);

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetObject();

  // awr result array as result document
  result_doc.GetObject() = sim_time_info.GetObject();

  return {DocumentToString(result_doc)};
}

crow::response Version() {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value version_info = NestGetVersion(json_alloc);

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetObject();

  // awr result array as result document
  result_doc.GetObject() = version_info.GetObject();

  return {DocumentToString(result_doc)};
}
} // namespace insite
