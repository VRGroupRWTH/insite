#include "data_storage.hpp"

#include <mpi.h>
#include <rapidjson/stringbuffer.h>
#include <sys/types.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <set>

#include "../serialize.hpp"
#include "kernel_manager.h"
#include "node.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "spatial.h"

namespace insite {

// We are only using C++11 :(
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace {
constexpr size_t TIME_DIMENSION = 0;
constexpr size_t NEURON_DIMENSION = 1;
}  // namespace

std::ostream& operator<<(std::ostream& os, const NodeCollection& c) {
  os << "first node: " << c.first_node_id << " node count: " << c.node_count << " model name: " << c.model_name << " model status: " << c.model_status;
  return os;
}

DataStorage::DataStorage() {
  SetCurrentSimulationTime(0.0);
}

void DataStorage::Reset() {
  spikedetectors_.clear();
  multimeters_.clear();
  node_collections_.clear();
}

NodeCollection ReceiveNodeCollection(int source, int tag = 0) {
  NodeCollection received_collection;
  MPI_Status status;

  // Fixed upper length for string for simplicity. If needed use MPI_Probe to get actual size and alloc correct amount of memory.
  char string_buffer[1024] = {};
  // int parameter_vector_size;

  MPI_Recv(&received_collection.first_node_id, 1, MPI_UINT64_T, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Recv(&received_collection.node_count, 1, MPI_UINT64_T, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // Receive the model name string. Buffered into a raw char array and then converted to std::string
  MPI_Recv(&string_buffer, sizeof(string_buffer), MPI_CHAR, source, 0, MPI_COMM_WORLD, &status);
  received_collection.model_name = std::string(string_buffer);

  memset(string_buffer, 0, sizeof(string_buffer));  // Clear the receive buffer before reusing the buffer
  MPI_Recv(&string_buffer, sizeof(string_buffer), MPI_CHAR, source, 0, MPI_COMM_WORLD, &status);
  received_collection.model_status = string_buffer;
  return received_collection;
}

void SendNodeCollection(const NodeCollection& node_collection, int dest = 0, int tag = 0) {
  MPI_Send(&node_collection.first_node_id, 1, MPI_UINT64_T, dest, tag, MPI_COMM_WORLD);

  MPI_Send(&node_collection.node_count, 1, MPI_UINT64_T, dest, tag, MPI_COMM_WORLD);
  MPI_Send(node_collection.model_name.c_str(), node_collection.model_name.size(), MPI_CHAR, dest, tag, MPI_COMM_WORLD);

  const auto serialized_model_status = node_collection.model_status;
  MPI_Send(serialized_model_status.c_str(), serialized_model_status.size(), MPI_CHAR, dest, tag, MPI_COMM_WORLD);
}

void SendNodeCollections(const std::vector<NodeCollection>& node_collections, int dest = 0, int tag = 0) {
  auto collection_size = node_collections.size();
  MPI_Send(&collection_size, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
  for (const auto& node_collection : node_collections)
    SendNodeCollection(node_collection, dest, tag);
}

void DataStorage::SetNodesFromCollection(const nest::NodeCollectionPTR& local_node_collection) {
  std::unique_lock<std::mutex> lock(node_collections_mutex_);
  std::set<nest::NodeCollection*> node_handles_node_connections;
  node_collections_.clear();
  nodes_.clear();
  nodes_.reserve(local_node_collection->size());

  DictionaryDatum node_properties(new Dictionary());

  // web::json::value node = web::json::value::object();
  for (const nest::NodeIDTriple& node_id_triple : *local_node_collection.get()) {
    nest::Node* nest_node = nest::kernel().node_manager.get_node_or_proxy(node_id_triple.node_id);
    nest::NodeCollectionPTR node_collection = nest_node->get_nc();

    // This is a null pointer for nodes simulated on other MPI ranks. However, as only the
    // local nodes should be passed into this function, this should never happen!
    // EXCEPT for nodes representing multimeters, spike recorders, generators, etc..
    // They are present ob every rank but only have a collection set on their "home" rank.
    if (node_collection == nullptr) {
      continue;
    }

    // Check if we already handles this node collection
    if (node_handles_node_connections.count(node_collection.get()) == 0) {
      assert(node_collection->is_range());
      assert(node_collection->size() > 0);

      std::string model_name;
      std::string serialized_model_status;
      rapidjson::StringBuffer model_status_buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(model_status_buffer);
      if (node_id_triple.model_id != nest::invalid_index) {
        nest::Model* model = nest::kernel().model_manager.get_node_model(node_id_triple.model_id);
        if (model != nullptr) {
          model_name = model->get_name();

          // segfaults for some reason:
          auto model_status = model->get_status();
          if (model_status.valid()) {
            SerializeDatum(model_status, writer);
            serialized_model_status = model_status_buffer.GetString();
          }
        }
      }

      node_collections_.push_back({(*node_collection)[0], node_collection->size(), model_name, serialized_model_status});
      node_handles_node_connections.insert(node_collection.get());
    }
  }
  int comm_size_world;
  int mpi_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size_world);

  std::sort(node_collections_.begin(), node_collections_.end());
  node_collections_.erase(std::unique(node_collections_.begin(), node_collections_.end()), node_collections_.end());

  if (mpi_rank == 0) {
    for (int i = 1; i < comm_size_world; i++) {
      auto node_collection = ReceiveCollectionsFromNode(i);
      node_collections_.insert(node_collections_.end(), node_collection.begin(), node_collection.end());
    }

    std::sort(node_collections_.begin(), node_collections_.end());
    node_collections_.erase(std::unique(node_collections_.begin(), node_collections_.end()), node_collections_.end());

    for (int i = 1; i < comm_size_world; i++) {
      {
        SendNodeCollections(node_collections_, i);
      }
    }
  } else {
    SendNodeCollections(node_collections_);
    auto node_collections_complete = ReceiveCollectionsFromNode(0);
    node_collections_.clear();
    node_collections_.insert(node_collections_.end(), node_collections_complete.begin(), node_collections_complete.end());
  }

  for (const nest::NodeIDTriple& node_id_triple : *local_node_collection.get()) {
    rapidjson::StringBuffer node_json_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(node_json_buffer);
    writer.StartObject();

    SerializeNode(writer, node_id_triple);
    writer.EndObject();
    nodes_.insert(std::make_pair(node_id_triple.node_id, node_json_buffer.GetString()));
  }
}

uint64_t DataStorage::find_node_collection_index_for_node_id(const uint64_t node_id) const {
  for (size_t i = 0; i < node_collections_.size(); ++i) {
    if (node_id >= node_collections_[i].first_node_id && node_id < node_collections_[i].first_node_id + node_collections_[i].node_count) {
      return i;
    }
  }
  throw std::runtime_error("Invalid node id");
};

void DataStorage::SerializeNode(rapidjson::Writer<rapidjson::StringBuffer>& writer, const nest::NodeIDTriple& node_id_triple) const {
  const size_t node_collection_index = find_node_collection_index_for_node_id(node_id_triple.node_id);
  const NodeCollection& node_collection = node_collections_[node_collection_index];

  if (!nest::kernel().node_manager.is_local_node_id(node_id_triple.node_id)) {
    return;
  }

  writer.Key("nodeId");
  writer.Uint(node_id_triple.node_id);

  writer.Key("simulationNodeId");
  writer.Uint(nest::kernel().get_kernel_manager().mpi_manager.get_rank());

  DictionaryDatum node_status = nest::kernel().node_manager.get_status(node_id_triple.node_id);
  writer.Key("nodeStatus");
  SerializeDatum(node_status, writer);

  // Check if the node has spatial positions attached to it.
  // This should probably be replaced by something more accessible
  // than checking against NaN.
  std::vector<double> position = nest::get_position(node_id_triple.node_id);
  writer.Key("position");
  if (!std::isnan(position[0])) {
    writer.StartArray();
    for (const auto& pos : position) {
      writer.Double(pos);
    }
    writer.EndArray();
  } else {
    writer.Null();
  }
  // Be careful because of these nodes that are not actually local (see above).
  // auto node_iterator = nodes_.find(node_id_triple.node_id);
  // if (node_iterator != nodes_.end()) {
  writer.Key("model");
  writer.String(node_collection.model_name.c_str());
  writer.Key("nodeCollectionId");
  writer.Uint(node_collection_index);
  // node_iterator->second = node_json_buffer.GetString();
  // }
}

uint64_t DataStorage::GetNodeCollectionIdForNodeId(uint64_t node_id) const {
  std::unique_lock<std::mutex> lock(node_collections_mutex_);
  return GetNodeCollectionIdForNodeIdNoLock(node_id);
}

std::shared_ptr<SpikedetectorStorage> DataStorage::CreateSpikeDetectorStorage(std::uint64_t spike_detector_id) {
  std::unique_lock<std::mutex> lock(spikedetectors_mutex_);
  auto spike_detector_iterator = spikedetectors_.find(spike_detector_id);
  if (spike_detector_iterator == spikedetectors_.end()) {
    auto insert_result = spikedetectors_.insert(std::make_pair(spike_detector_id, std::make_shared<SpikedetectorStorage>(spike_detector_id)));
    assert(insert_result.second);
    return insert_result.first->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SpikedetectorStorage> DataStorage::GetSpikeDetectorStorage(std::uint64_t spike_detector_id) {
  std::unique_lock<std::mutex> lock(spikedetectors_mutex_);
  auto spike_detector_iterator = spikedetectors_.find(spike_detector_id);
  return spike_detector_iterator == spikedetectors_.end() ? nullptr : spike_detector_iterator->second;
}

std::shared_ptr<MultimeterStorage> DataStorage::CreateMultimeterStorage(std::uint64_t multimeter_id) {
  std::unique_lock<std::mutex> lock(multimeters_mutex_);
  auto multimeter_iterator = multimeters_.find(multimeter_id);
  if (multimeter_iterator == multimeters_.end()) {
    auto insert_result = multimeters_.insert(std::make_pair(multimeter_id, std::make_shared<MultimeterStorage>(multimeter_id)));
    assert(insert_result.second);
    return insert_result.first->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<MultimeterStorage> DataStorage::GetMultimeterStorage(std::uint64_t multimeter_id) {
  std::unique_lock<std::mutex> lock(multimeters_mutex_);
  auto multimeter_iterator = multimeters_.find(multimeter_id);
  return multimeter_iterator == multimeters_.end() ? nullptr : multimeter_iterator->second;
}

void DataStorage::AddSpike(std::uint64_t spikedetector_id, double simulation_time, std::uint64_t node_id) {
  GetSpikeDetectorStorage(spikedetector_id)->AddSpike(simulation_time, node_id);
}

void DataStorage::AddMultimeterMeasurement(std::uint64_t multimeter_id, double simulation_time, std::uint64_t node_id, const std::vector<double>& double_values, const std::vector<long>& long_values) {
  GetMultimeterStorage(multimeter_id)->AddMeasurement(simulation_time, node_id, double_values, long_values);
}

void DataStorage::SetCurrentSimulationTime(double simulation_time) {
  uint64_t simulation_time_int;
  memcpy(&simulation_time_int, &simulation_time, sizeof(simulation_time_int));
  current_simulation_time_ = simulation_time_int;
}

void DataStorage::SetSimulationTimeRange(double begin, double end) {
  uint64_t simulation_time_int;
  memcpy(&simulation_time_int, &begin, sizeof(simulation_time_int));
  simulation_begin_time_ = simulation_time_int;

  memcpy(&simulation_time_int, &end, sizeof(simulation_time_int));
  simulation_end_time_ = simulation_time_int;
}

double DataStorage::GetCurrentSimulationTime() const {
  const uint64_t simulation_time_int = current_simulation_time_;
  double simulation_time;
  memcpy(&simulation_time, &simulation_time_int, sizeof(simulation_time));
  return simulation_time;
}

double DataStorage::GetSimulationBeginTime() const {
  const uint64_t simulation_time_int = simulation_begin_time_;
  double simulation_time;
  memcpy(&simulation_time, &simulation_time_int, sizeof(simulation_time));
  return simulation_time;
}

double DataStorage::GetSimulationEndTime() const {
  const uint64_t simulation_time_int = simulation_end_time_;
  double simulation_time;
  memcpy(&simulation_time, &simulation_time_int, sizeof(simulation_time));
  return simulation_time;
}

uint64_t DataStorage::GetNodeCollectionIdForNodeIdNoLock(uint64_t node_id) const {
  for (uint64_t node_collection_id = 0; node_collection_id < node_collections_.size(); ++node_collection_id) {
    const auto& node_collection = node_collections_[node_collection_id];
    if (node_collection.first_node_id <= node_id && node_id < node_collection.first_node_id + node_collection.node_count) {
      return node_collection_id;
    }
  }
  return 0xffffffffffffffff;
}
std::vector<NodeCollection> DataStorage::ReceiveCollectionsFromNode(int source) {
  std::vector<NodeCollection> tmp_node_collection;
  int comm_size_world;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size_world);

  int number_of_collections = 0;
  MPI_Recv(&number_of_collections, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  for (int j = 0; j < number_of_collections; j++) {
    auto tmp = ReceiveNodeCollection(source);
    tmp_node_collection.push_back(tmp);
  }

  return tmp_node_collection;
}

}  // namespace insite
