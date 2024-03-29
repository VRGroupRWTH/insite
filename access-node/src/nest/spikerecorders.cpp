#include <nest/json_strings.h>
#include <nest/spikerecorders.h>
#include <utility>

namespace insite {

void CheckSpikeRecorderDataValid(const rapidjson::GenericObject<false, rapidjson::Value>& json_spikerecorder) {
  assert(json_spikerecorder.HasMember(json_strings::kSpikerecorderId) &&
         json_spikerecorder[json_strings::kSpikerecorderId].IsInt());
  assert(json_spikerecorder.HasMember(json_strings::kNodeIds) &&
         json_spikerecorder[json_strings::kNodeId].IsArray());
}

std::vector<SpikerecorderContainer> GetSpikerecorder(const rapidjson::Value& json_spikerecorders) {
  std::vector<SpikerecorderContainer> spikerecorders;

  for (const auto& recorder : json_spikerecorders.GetArray()) {
    spikerecorders.emplace_back(recorder);
  }

  return spikerecorders;
}

crow::response SpikeRecorders(int api_version) {
  rapidjson::StringBuffer json_buffer;
  rapidjson::Writer<rapidjson::StringBuffer> json_writer(json_buffer);

  rapidjson::MemoryPoolAllocator<> json_alloc;

  std::string endpoint = "/spikerecorders";

  auto response = QueryRandomNode(ServerConfig::GetInstance().request_nest_urls,
                                  endpoint, api_version);
  if (response.status_code == 202) {
    return crow::response(202);
  }

  std::string response_text = response.text;
  rapidjson::Document json_spikerecorders;
  json_spikerecorders.Parse(response_text.c_str());

  if (api_version == 1) {
    json_writer.StartArray();
    auto spikerecorder = GetSpikerecorder(json_spikerecorders);
    for (const auto& spikerecorder : spikerecorder) {
      spikerecorder.WriteToJson(json_writer);
    }
    json_writer.EndArray();

    return {json_buffer.GetString()};
  }
  if (api_version == 2) {
    auto spikerecorder = GetSpikerecorder(json_spikerecorders["spikerecorder"]);
    std::string sim_id = json_spikerecorders.GetObject()["simId"].GetString();

    json_writer.StartObject();
    json_writer.Key("simId");
    json_writer.String(sim_id.c_str());
    json_writer.Key("spikerecorders");
    json_writer.StartArray();
    for (const auto& spikerecorder : spikerecorder) {
      spikerecorder.WriteToJson(json_writer);
    }
    json_writer.EndArray();
    json_writer.EndObject();

    return {json_buffer.GetString()};
  }
  return crow::response(202);
}

crow::response SpikesBySpikeRecorderId(const crow::request& req, int api_version, int requested_spikerecorder_id) {
  SpikeParameter params(req.url_params);
  params.spike_detector_id = requested_spikerecorder_id;

  // rapidjson::StringBuffer buffer;
  // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  //
  // SpikeContainer spikes = std::move(NestGetSpikes(params, api_version));
  // if (api_version == 1) {
  //
  //   spikes.SerializeToJson(writer, params.skip, params.top,
  //                          params.reverse_order, true);
  // }
  //
  // if (api_version == 2) {
  //   writer.StartObject();
  //   writer.Key("simId");
  //   writer.String("xx:xx");
  //   writer.Key("spikes");
  //   spikes.SerializeToJson(writer, params.skip, params.top,
  //                          params.reverse_order, true);
  // }
  //
  // SPDLOG_INFO("Serialized in {}", sw_serialize.elapsed());

  // return {buffer.GetString()};
  return {NestGetSpikes(params, api_version)};
}

}  // namespace insite
