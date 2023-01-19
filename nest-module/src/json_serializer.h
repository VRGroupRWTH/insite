#include <vector>
#include "extern/rapidjson/include/rapidjson/writer.h"
#include "storage/data_storage.hpp"
#include "storage/multimeter_storage.hpp"
#include "storage/spikedetector_storage.hpp"
namespace insite {
namespace json_serializer {

void Spikes(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<Spike>& spikes, bool last_frame);
void Version(rapidjson::Writer<rapidjson::StringBuffer>& writer);
void SimulationTimeInfo(rapidjson::Writer<rapidjson::StringBuffer>& writer, const SimulationTimeInfo info);
void NodeCollections(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<NodeCollection>& node_collections);
void SpikeDetectors(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>>& spikedetectors);
void Multimeters(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::unordered_map<std::uint64_t, std::shared_ptr<MultimeterStorage>>& multimeters);

}  // namespace json_serializer
}  // namespace insite
