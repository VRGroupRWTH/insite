#include "data_storage.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <set>

#include "node.h"
#include "kernel_manager.h"

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

void DataStorage::SetNodesFromCollection(
    const nest::NodeCollectionPTR& node_collection) {
  std::unique_lock<std::mutex> lock(node_collections_mutex_);
  std::set<nest::NodeCollection*> node_handles_node_connections;
  node_collections_.clear();

  for (const nest::NodeIDTriple& node_id_triple : *node_collection.get()) {
    nest::Node* node = nest::kernel().node_manager.get_node_or_proxy(node_id_triple.node_id);
    nest::NodeCollectionPTR node_collection = node->get_nc();

    if (node_handles_node_connections.count(node_collection.get()) == 0) {
      assert(node_collection->is_range());
      assert(node_collection->size() > 0);

      std::string model =
          node_id_triple.model_id != nest::invalid_index
              ? nest::kernel().model_manager.get_model(node_id_triple.model_id)->get_name()
              : "none";

      node_collections_.push_back({
        (*node_collection)[0],
        node_collection->size(),
        model
      });

      std::cout << "Node collection: [" << (*node_collection)[0] << ","
                << (*node_collection)[0] + node_collection->size()
                << "): " << model << std::endl;

      node_handles_node_connections.insert(node_collection.get());
    }
  }
}

std::shared_ptr<SpikedetectorStorage> DataStorage::CreateSpikeDetectorStorage(std::uint64_t spike_detector_id) {
  std::unique_lock<std::mutex> lock(spikedetectors_mutex_);
  auto spike_detector_iterator = spikedetectors_.find(spike_detector_id);
  if (spike_detector_iterator == spikedetectors_.end()) {
    auto insert_result = spikedetectors_.insert(
        std::make_pair(spike_detector_id,
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
  return spike_detector_iterator == spikedetectors_.end() ? nullptr : spike_detector_iterator->second;
}

void DataStorage::AddSpike(std::uint64_t spikedetector_id,
                           double simulation_time, std::uint64_t neuron_id) {
  GetSpikeDetectorStorage(spikedetector_id)
      ->AddSpike(simulation_time, neuron_id);
}

void DataStorage::AddMultimeterMeasurement(std::uint64_t device_id,
                                           const std::string& attribute_name,
                                           const double simulation_time,
                                           const std::uint64_t gid,
                                           const double value) {
  std::unique_lock<std::mutex> lock(measurement_mutex_);
  auto& measurement = buffered_measurements_[device_id][attribute_name];
  auto& simulation_times = measurement.simulation_times;
  auto& gids = measurement.gids;
  auto& values = measurement.values;

  auto time_iterator = std::lower_bound(
      simulation_times.begin(), simulation_times.end(), simulation_time);
  auto time_index = std::distance(simulation_times.begin(), time_iterator);
  if (time_iterator == simulation_times.end() ||
      *time_iterator != simulation_time) {
    simulation_times.insert(time_iterator, simulation_time);

    auto new_values =
        std::vector<double>(simulation_times.size() * gids.size(), 0.0);
    for (std::size_t t = 0; t < simulation_times.size(); ++t)
      for (std::size_t g = 0; g < gids.size(); ++g)
        if (t != time_index)
          new_values[t * gids.size() + g] =
              values[(t > time_index ? t - 1 : t) * gids.size() + g];
    values = new_values;
  }

  auto gid_iterator = std::lower_bound(gids.begin(), gids.end(), gid);
  auto gid_index = std::distance(gids.begin(), gid_iterator);
  if (gid_iterator == gids.end() || *gid_iterator != gid) {
    gids.insert(gid_iterator, gid);

    auto new_values =
        std::vector<double>(simulation_times.size() * gids.size(), 0.0);
    for (std::size_t t = 0; t < simulation_times.size(); ++t)
      for (std::size_t g = 0; g < gids.size(); ++g)
        if (g != gid_index)
          new_values[t * gids.size() + g] =
              values[t * gids.size() + (g > gid_index ? g - 1 : g)];
    values = new_values;
  }

  values[time_index * gids.size() + gid_index] = value;
}

std::unordered_map<std::uint64_t,
                   std::unordered_map<std::string, MultimeterMeasurements>>
DataStorage::GetMultimeterMeasurements() {
  std::unique_lock<std::mutex> lock(measurement_mutex_);
  auto measurements = buffered_measurements_;
  return measurements;
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

}  // namespace insite
