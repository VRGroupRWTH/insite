#include "multimeter_storage.hpp"

#include <algorithm>
#include <cassert>

#include "kernel_manager.h"

namespace insite {

MultimeterStorage::MultimeterStorage(std::uint64_t spikedetector_id,
                                     std::uint64_t timestep_ring_buffer_size)
    : id_(spikedetector_id) {
  simulation_times_.reserve(timestep_ring_buffer_size);
}

void MultimeterStorage::SetAttributes(const std::vector<Name>& double_attributes,
                                      const std::vector<Name>& long_attributes) {
  std::unique_lock<std::mutex> lock(mutex_);

  double_attributes_.clear();
  double_attributes_.reserve(double_attributes.size());
  long_attributes_.clear();
  long_attributes_.reserve(long_attributes.size());
  attributes_.clear();

  for (const Name& double_attribute : double_attributes) {
    const std::string attribute_name = double_attribute.toString();
    auto attribute_storage = std::make_shared<AttributeStorage<double>>(attribute_name, simulation_times_.capacity());
    double_attributes_.push_back(attribute_storage);
    attributes_.insert(std::make_pair(attribute_name, attribute_storage));
  }

  for (const Name& long_attribute : long_attributes) {
    const std::string attribute_name = long_attribute.toString();
    auto attribute_storage = std::make_shared<AttributeStorage<long>>(attribute_name, simulation_times_.capacity());
    long_attributes_.push_back(attribute_storage);
    attributes_.insert(std::make_pair(attribute_name, attribute_storage));
  }
}

void MultimeterStorage::Prepare(
    const nest::NodeCollectionPTR& all_nodes) {
  std::unique_lock<std::mutex> lock(mutex_);
  nest::NodeCollectionPTR multimeter_collection =
      std::make_shared<nest::NodeCollectionPrimitive>(id_, id_);

  std::deque<nest::ConnectionID> connections;
  for (nest::synindex synapse_id = 0;
       synapse_id < nest::kernel().model_manager.get_num_synapse_prototypes();
       ++synapse_id) {
    nest::kernel().connection_manager.get_connections(
        connections, multimeter_collection, all_nodes, synapse_id, -1);
  }

  connected_nodes_.resize(0);
  connected_nodes_.reserve(connections.size());
  for (const auto& connection : connections) {
    connected_nodes_.push_back(connection.get_target_node_id());
  }

  for (auto& attribute_storage : attributes_) {
    attribute_storage.second.lock()->SetNodeCount(connected_nodes_.size());
  }
  std::sort(connected_nodes_.begin(), connected_nodes_.end());
}

void MultimeterStorage::ExtractConnectedNodeIds(std::vector<std::uint64_t>* node_ids) {
  assert(node_ids != nullptr);
  std::unique_lock<std::mutex> lock(mutex_);

  *node_ids = connected_nodes_;
}

void MultimeterStorage::AddMeasurement(double simulation_time, std::uint64_t node_id,
                                       const std::vector<double>& double_values,
                                       const std::vector<long>& long_values) {
  std::unique_lock<std::mutex> lock(mutex_);

  // Find index of the node
  const auto equal_range = std::equal_range(connected_nodes_.begin(), connected_nodes_.end(), node_id);
  assert(std::distance(equal_range.first, equal_range.second) == 1);

  const uint64_t node_offset = equal_range.first - connected_nodes_.begin();
  const uint64_t time_offset = GetTimeOffsetNoLock(simulation_time);

  assert(double_values.size() == double_attributes_.size());
  for (size_t i = 0; i < double_values.size(); ++i) {
    double_attributes_[i]->AddMeasurement(time_offset, node_offset, double_values[i]);
  }

  assert(long_values.size() == long_attributes_.size());
  for (size_t i = 0; i < long_values.size(); ++i) {
    long_attributes_[i]->AddMeasurement(time_offset, node_offset, long_values[i]);
  }
}

namespace {

template <typename T>
web::json::value ToJsonArray(const std::vector<T>& vector) {
  web::json::value array = web::json::value::array(vector.size());

  for (size_t i = 0; i < vector.size(); ++i) {
    array[i] = vector[i];
  }

  return array;
}

}  // namespace

web::json::value MultimeterStorage::Serialize() const {
  web::json::value serialized_object = web::json::value::object();
  serialized_object["multimeterId"] = id_;

  {
    std::unique_lock<std::mutex> lock(mutex_);

    serialized_object["attributes"] = web::json::value::array(attributes_.size());
    serialized_object["nodeIds"] = ToJsonArray(connected_nodes_);
    size_t current_index = 0;
    for (const auto attribute : attributes_) {
      serialized_object["attributes"][current_index] = web::json::value(attribute.first);
      ++current_index;
    }
  }

  return serialized_object;
}

web::json::value MultimeterStorage::ExtractMeasurements(const std::string& attribute_name, const std::vector<uint64_t>& node_ids, double from_time, double to_time) {
  auto attribute_storage_iterator = attributes_.find(attribute_name);
  if (attribute_storage_iterator == attributes_.end()) {
    return {};
  }

  const auto& nodes_to_query = node_ids.size() > 0 ? node_ids : connected_nodes_;
  std::vector<uint64_t> nodes;
  std::vector<uint64_t> node_indices;
  nodes.reserve(nodes_to_query.size());
  node_indices.reserve(nodes_to_query.size());

  // TODO: in the case that `nodes == connected_nodes_` this can be heavily optimized
  for (uint64_t node : nodes_to_query) {
    auto range = std::equal_range(connected_nodes_.begin(), connected_nodes_.end(), node);
    if (range.first != range.second) {
      nodes.push_back(node);
      node_indices.push_back(std::distance(connected_nodes_.begin(), range.first));
    }
  }

  std::vector<uint64_t> time_indices;
  std::vector<double> times;
  time_indices.reserve(simulation_times_.size());
  times.reserve(simulation_times_.size());
  for (size_t i = 0; i < simulation_times_.size(); ++i) {
    const uint64_t time_index = (first_time_index_ + i) % simulation_times_.size();
    const double time = simulation_times_[time_index];

    if (time >= to_time) {
      break;
    } else if (time >= from_time) {
      time_indices.push_back(time_index);
      times.push_back(time);
    }
  }

  web::json::value measurements = web::json::value::object();
  std::shared_ptr<AttributeStorageBase> attribute_storage = attribute_storage_iterator->second.lock();
  measurements["simulationTimes"] = ToJsonArray(times);
  measurements["nodeIds"] = ToJsonArray(nodes);
  measurements["values"] = attribute_storage->ExtractValues(time_indices, node_indices);

  return measurements;
}

double MultimeterStorage::GetLatestSimulationTimeNoLock() const {
  if (simulation_times_.size() == 0) {
    return -std::numeric_limits<double>::infinity();
  } else if (first_time_index_ == 0) {
    return simulation_times_.back();
  } else {
    return simulation_times_[first_time_index_ - 1];
  }
}

uint64_t MultimeterStorage::GetTimeOffsetNoLock(double simulation_time) {
  // This can be very likely implemented more nicely!

  uint64_t time_offset;

  if (GetLatestSimulationTimeNoLock() < simulation_time) {
    if (simulation_times_.size() < simulation_times_.capacity()) {
      assert(first_time_index_ == 0);

      time_offset = simulation_times_.size();
      simulation_times_.push_back(simulation_time);
    } else {
      assert(first_time_index_ < simulation_times_.size());

      time_offset = first_time_index_;
      simulation_times_[first_time_index_] = simulation_time;
      ++first_time_index_;
      if (first_time_index_ == simulation_times_.size()) {
        first_time_index_ = 0;
      }
    }

    for (auto& attribute_storage : attributes_) {
      attribute_storage.second.lock()->EraseTimestepValues(time_offset);
    }
  } else {
    time_offset = first_time_index_ > 0 ? first_time_index_ - 1 : simulation_times_.size() - 1;
  }

  return time_offset;
}

}  // namespace insite