#include "json_serializer.h"
#include "extern/rapidjson/include/rapidjson/stringbuffer.h"
#include "http_server.hpp"
#include "rapidjson/document.h"

namespace insite {
namespace json_serializer {

void SpikesV2(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<Spike>& spikes, bool last_frame) {
  auto sim_id = std::to_string(HttpServer::user_id) + ":" + std::to_string(HttpServer::run_no[HttpServer::user_id]);
  writer.StartObject();
  writer.Key("simId");
  writer.String(sim_id.c_str());
  writer.Key("simulationTimes");
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Double(spike.simulation_time);
  }
  writer.EndArray();

  writer.Key("nodeIds");
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Int(spike.node_id);
  }
  writer.EndArray();
  writer.Key("lastFrame");
  writer.Bool(last_frame);
  writer.EndObject();
}

void Spikes(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<Spike>& spikes, bool last_frame) {
  writer.StartObject();
  writer.Key("simulationTimes");
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Double(spike.simulation_time);
  }
  writer.EndArray();

  writer.Key("nodeIds");
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Int(spike.node_id);
  }
  writer.EndArray();
  writer.Key("lastFrame");
  writer.Bool(last_frame);
  writer.EndObject();
}

void SpikesAlt(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<Spike>& spikes, bool last_frame) {
  writer.StartObject();
  writer.Key("spikes");
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.StartArray();
    writer.Double(spike.simulation_time);
    writer.Int(spike.node_id);
    writer.EndArray();
  }
  writer.EndArray();

  writer.EndArray();
  writer.Key("lastFrame");
  writer.Bool(last_frame);
  writer.EndObject();
}

void Version(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
  rapidjson::Document document;
  document.SetObject();
  document.AddMember("version", "1.2", document.GetAllocator());

  document.Accept(writer);
}

void SimulationTimeInfo(rapidjson::Writer<rapidjson::StringBuffer>& writer, const struct SimulationTimeInfo info) {
  rapidjson::Document document(rapidjson::kObjectType);
  document.AddMember("current", info.current_time, document.GetAllocator());
  document.AddMember("begin", info.begin_time, document.GetAllocator());
  document.AddMember("end", info.end_time, document.GetAllocator());
  document.AddMember("stepSize", info.step_size, document.GetAllocator());

  document.Accept(writer);
}

void SimulationTimeInfoV2(rapidjson::Writer<rapidjson::StringBuffer>& writer, const struct SimulationTimeInfo info) {
  rapidjson::Document document(rapidjson::kObjectType);
  auto sim_id = std::to_string(HttpServer::user_id) + ":" + std::to_string(HttpServer::run_no[HttpServer::user_id]);
  auto sim_id_json = rapidjson::Value(sim_id.c_str(), document.GetAllocator());
  document.AddMember("simId", sim_id_json, document.GetAllocator());
  document.AddMember("current", info.current_time, document.GetAllocator());
  document.AddMember("begin", info.begin_time, document.GetAllocator());
  document.AddMember("end", info.end_time, document.GetAllocator());
  document.AddMember("stepSize", info.step_size, document.GetAllocator());

  document.Accept(writer);
}

void NodeCollectionsV2(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<NodeCollection>& node_collections) {
  auto sim_id = std::to_string(HttpServer::user_id) + ":" + std::to_string(HttpServer::run_no[HttpServer::user_id]);
  writer.StartObject();
  writer.Key("simId");
  writer.String(sim_id.c_str());
  writer.Key("nodeCollections");
  writer.StartArray();
  for (size_t i = 0; i < node_collections.size(); ++i) {
    writer.StartObject();
    writer.Key("nodeCollectionId");
    writer.Uint64(i);

    writer.Key("nodes");
    writer.StartObject();
    writer.Key("firstId");
    writer.Uint64(node_collections[i].first_node_id);
    writer.Key("lastId");
    writer.Uint64(node_collections[i].first_node_id + node_collections[i].node_count - 1);
    writer.Key("count");
    writer.Uint64(node_collections[i].node_count);
    writer.EndObject();

    writer.Key("model");
    writer.StartObject();
    writer.Key("name");
    writer.String(node_collections[i].model_name.c_str());
    writer.Key("status");
    const auto model_status = node_collections[i].model_status;
    writer.RawValue(model_status.c_str(), model_status.length(), rapidjson::kObjectType);
    writer.EndObject();

    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();
}

void NodeCollections(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<NodeCollection>& node_collections) {
  writer.StartArray();
  for (size_t i = 0; i < node_collections.size(); ++i) {
    writer.StartObject();
    writer.Key("nodeCollectionId");
    writer.Uint64(i);

    writer.Key("nodes");
    writer.StartObject();
    writer.Key("firstId");
    writer.Uint64(node_collections[i].first_node_id);
    writer.Key("lastId");
    writer.Uint64(node_collections[i].first_node_id + node_collections[i].node_count - 1);
    writer.Key("count");
    writer.Uint64(node_collections[i].node_count);
    writer.EndObject();

    writer.Key("model");
    writer.StartObject();
    writer.Key("name");
    writer.String(node_collections[i].model_name.c_str());
    writer.Key("status");
    const auto model_status = node_collections[i].model_status;
    writer.RawValue(model_status.c_str(), model_status.length(), rapidjson::kObjectType);
    writer.EndObject();

    writer.EndObject();
  }
  writer.EndArray();
}

void MultimetersV2(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::unordered_map<std::uint64_t, std::shared_ptr<MultimeterStorage>>& multimeters) {
  auto sim_id = std::to_string(HttpServer::user_id) + ":" + std::to_string(HttpServer::run_no[HttpServer::user_id]);

  writer.StartObject();
  writer.Key("simId");
  writer.String(sim_id.c_str());
  writer.Key("multimeter");
  writer.StartArray();

  for (const auto& multimeter_id_storage : multimeters) {
    multimeter_id_storage.second->Serialize(writer);
  }
  writer.EndArray();
  writer.EndObject();
}

void Multimeters(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::unordered_map<std::uint64_t, std::shared_ptr<MultimeterStorage>>& multimeters) {
  writer.StartArray();

  for (const auto& multimeter_id_storage : multimeters) {
    multimeter_id_storage.second->Serialize(writer);
  }
  writer.EndArray();
}

void SpikeDetectorsV2(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>>& spikedetectors) {
  writer.StartObject();
  writer.Key("simId");
  auto sim_id = std::to_string(HttpServer::user_id) + ":" + std::to_string(HttpServer::run_no[HttpServer::user_id]);
  writer.String(sim_id.c_str());

  writer.Key("spikerecorder");
  SpikeDetectors(writer, spikedetectors);
  writer.EndObject();
}
void SpikeDetectors(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>>& spikedetectors) {
  writer.StartArray();
  for (const auto& spikedetector_id_storage : spikedetectors) {
    std::vector<std::uint64_t> connected_node_ids;
    writer.StartObject();
    writer.Key("spikerecorderId");
    writer.Uint64(spikedetector_id_storage.first);

    spikedetector_id_storage.second->ExtractConnectedNodeIds(
        &connected_node_ids);

    writer.Key("nodeIds");
    writer.StartArray();
    for (size_t i = 0; i < connected_node_ids.size(); ++i) {
      writer.Uint64(connected_node_ids[i]);
    }
    writer.EndArray();
    writer.EndObject();
  }
  writer.EndArray();
}

}  // namespace json_serializer
}  // namespace insite
