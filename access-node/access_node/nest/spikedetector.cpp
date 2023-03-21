#include "spikedetector.h"

#include <utility>
#include "jsonStrings.h"

namespace insite {

Spikerecorder::Spikerecorder() {}

Spikerecorder::Spikerecorder(const rapidjson::Value& document)
    : spikerecorder_id(document[json_strings::kSpikerecorderId].GetUint64()) {
  auto json_nodes = document[json_strings::kNodeIds].GetArray();
  nodes.resize(json_nodes.Size());
  for (std::uint64_t i = 0; i < json_nodes.Size(); i++) {
    nodes[i] = json_nodes[i].GetUint64();
  }
}

Spikerecorder::Spikerecorder(const rapidjson::Value& document,
                             std::string sim_id)
    : sim_id(std::move(sim_id)),
      spikerecorder_id(document[json_strings::kSpikerecorderId].GetUint64()) {
  auto json_nodes = document[json_strings::kNodeIds].GetArray();
  nodes.resize(json_nodes.Size());
  for (std::uint64_t i = 0; i < json_nodes.Size(); i++) {
    nodes[i] = json_nodes[i].GetUint64();
  }
}

Spikerecorder::~Spikerecorder() {}

void Spikerecorder::WriteToJson(
    rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
  writer.StartObject();

  writer.Key("spikerecorderId");
  writer.Uint64(spikerecorder_id);

  writer.Key("nodeIds");
  writer.StartArray();
  for (auto node : nodes) {
    writer.Uint64(node);
  }
  writer.EndArray();
  writer.EndObject();
}

void Spikerecorder::ParseFromJson(rapidjson::Document input) {}

std::ostream& operator<<(std::ostream& ostream,
                         const Spikerecorder& spikerecorder) {
  ostream << "Spikerecorder: {" << std::endl;
  ostream << "spikerecorderId: " << spikerecorder.spikerecorder_id << std::endl;
  ostream << ", nodes: [";
  for (auto node : spikerecorder.nodes) {
    ostream << node << ",";
  }
  ostream << "]" << std::endl;
  ostream << "}" << std::endl;
  return ostream;
}

}  // namespace insite
