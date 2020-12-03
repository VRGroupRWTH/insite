#include "data_storage.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <set>

#include "../serialize.hpp"
#include "kernel_manager.h"
#include "node.h"
#include "topology.h"

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

DataStorage::DataStorage() { SetCurrentSimulationTime(0.0); }

void DataStorage::SetNodesFromCollection(const nest::NodeCollectionPTR& node_collection) {
  std::unique_lock<std::mutex> lock(node_collections_mutex_);
  std::set<nest::NodeCollection*> node_handles_node_connections;
  node_collections_.clear();
  nodes_.clear();
  nodes_.reserve(node_collection->size());

  DictionaryDatum node_properties(new Dictionary());

  for (const nest::NodeIDTriple& node_id_triple : *node_collection.get()) {
    nest::Node* node = nest::kernel().node_manager.get_node_or_proxy(node_id_triple.node_id);
    nest::NodeCollectionPTR node_collection = node->get_nc();

    // This is a null pointer for nodes simulated on other MPI ranks
    if (node_collection) {
      std::cout << "[insite] ";
      node_collection->print_me(std::cout);
      std::cout << std::endl;
    } else {
      std::cout << "[insite] No node collection!" << std::endl;
      continue;
    }

    if (node_handles_node_connections.count(node_collection.get()) == 0) {
      assert(node_collection->is_range());
      assert(node_collection->size() > 0);

      std::string model_name;
      std::vector<std::string> model_parameters;
      if (node_id_triple.model_id != nest::invalid_index) {
        nest::Model* model =
            nest::kernel().model_manager.get_model(node_id_triple.model_id);
        if (model != nullptr) {
          model_name = model->get_name();

          // segfaults for some reason:
          // auto model_status = model->get_status();
          // if (model_status.valid()) {
          //   for (const auto& datum : *model->get_status()) {
          //     std::cout << datum.first << ": ";
          //     datum.second.pprint(std::cout);
          //     std::cout << std::endl;
          //   }
          // }
        }
      }

      node_collections_.push_back({(*node_collection)[0],
                                   node_collection->size(), model_name,
                                   model_parameters});

      node_handles_node_connections.insert(node_collection.get());
    }
  }
}

uint64_t DataStorage::GetNodeCollectionIdForNodeId(uint64_t node_id) const {
  std::unique_lock<std::mutex> lock(node_collections_mutex_);
  return GetNodeCollectionIdForNodeIdNoLock(node_id);
}

std::shared_ptr<SpikedetectorStorage> DataStorage::CreateSpikeDetectorStorage(
    std::uint64_t spike_detector_id) {
  std::unique_lock<std::mutex> lock(spikedetectors_mutex_);
  auto spike_detector_iterator = spikedetectors_.find(spike_detector_id);
  if (spike_detector_iterator == spikedetectors_.end()) {
    auto insert_result = spikedetectors_.insert(std::make_pair(
        spike_detector_id,
        std::make_shared<SpikedetectorStorage>(spike_detector_id)));
    assert(insert_result.second);
    return insert_result.first->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SpikedetectorStorage> DataStorage::GetSpikeDetectorStorage(
    std::uint64_t spike_detector_id) {
  std::unique_lock<std::mutex> lock(spikedetectors_mutex_);
  auto spike_detector_iterator = spikedetectors_.find(spike_detector_id);
  return spike_detector_iterator == spikedetectors_.end()
             ? nullptr
             : spike_detector_iterator->second;
}

std::shared_ptr<MultimeterStorage> DataStorage::CreateMultimeterStorage(
    std::uint64_t multimeter_id) {
  std::unique_lock<std::mutex> lock(multimeters_mutex_);
  auto multimeter_iterator = multimeters_.find(multimeter_id);
  if (multimeter_iterator == multimeters_.end()) {
    auto insert_result = multimeters_.insert(std::make_pair(
        multimeter_id,
        std::make_shared<MultimeterStorage>(multimeter_id)));
    assert(insert_result.second);
    return insert_result.first->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<MultimeterStorage> DataStorage::GetMultimeterStorage(
    std::uint64_t multimeter_id) {
  std::unique_lock<std::mutex> lock(multimeters_mutex_);
  auto multimeter_iterator = multimeters_.find(multimeter_id);
  return multimeter_iterator == multimeters_.end()
             ? nullptr
             : multimeter_iterator->second;
}

void DataStorage::AddSpike(std::uint64_t spikedetector_id,
                           double simulation_time, std::uint64_t node_id) {
  GetSpikeDetectorStorage(spikedetector_id)
      ->AddSpike(simulation_time, node_id);
}

void DataStorage::AddMultimeterMeasurement(std::uint64_t multimeter_id, double simulation_time, std::uint64_t node_id,
                                           const std::vector<double>& double_values, const std::vector<long>& long_values) {
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
  const uint64_t simulation_time_int = simulation_end_time_;
  double simulation_time;
  memcpy(&simulation_time, &simulation_time_int, sizeof(simulation_time));
  return simulation_time;
}

double DataStorage::GetSimulationEndTime() const {
  const uint64_t simulation_time_int = simulation_begin_time_;
  double simulation_time;
  memcpy(&simulation_time, &simulation_time_int, sizeof(simulation_time));
  return simulation_time;
}

uint64_t DataStorage::GetNodeCollectionIdForNodeIdNoLock(
    uint64_t node_id) const {
  for (uint64_t node_collection_id = 0;
       node_collection_id < node_collections_.size(); ++node_collection_id) {
    const auto& node_collection = node_collections_[node_collection_id];
    if (node_collection.first_node_id <= node_id &&
        node_id < node_collection.first_node_id + node_collection.node_count) {
      return node_collection_id;
    }
  }
  return 0xffffffffffffffff;
}

}  // namespace insite
