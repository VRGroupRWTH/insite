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
  for (const Name& double_attribute : double_attributes) {
    double_attributes_.emplace_back(double_attribute.toString(), simulation_times_.capacity());
  }

  long_attributes_.clear();
  long_attributes_.reserve(long_attributes.size());
  for (const Name& long_attribute : long_attributes) {
    long_attributes_.emplace_back(long_attribute.toString(), simulation_times_.capacity());
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

  for (auto& double_attribute : double_attributes_) {
    double_attribute.SetNodeCount(connected_nodes_.size());
  }
  for (auto& long_attribute : long_attributes_) {
    long_attribute.SetNodeCount(connected_nodes_.size());
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
  const auto equal_range = std::equal_range(connected_nodes_.begin(), connected_nodes_.end(), node_id);
  assert(std::distance(equal_range.first, equal_range.second) == 1);

  const uint64_t node_offset = equal_range.first - connected_nodes_.begin();
  const uint64_t time_offset = GetTimeOffsetNoLock(simulation_time);

  assert(double_values.size() == double_attributes_.size());
  for (size_t i = 0; i < double_values.size(); ++i) {
    double_attributes_[i].AddMeasurement(time_offset, node_offset, double_values[i]);
  }

  assert(long_values.size() == long_attributes_.size());
  for (size_t i = 0; i < long_values.size(); ++i) {
    long_attributes_[i].AddMeasurement(time_offset, node_offset, long_values[i]);
  }
}

web::json::value MultimeterStorage::Serialize() const {
  web::json::value serialized_object = web::json::value::object();
  serialized_object["multimeterId"] = id_;

  {
    std::unique_lock<std::mutex> lock(mutex_);

    serialized_object["attributes"] = web::json::value::array(double_attributes_.size() + long_attributes_.size());
    for (size_t i = 0; i < double_attributes_.size(); ++i) {
      serialized_object["attributes"][i] = web::json::value(double_attributes_[i].name());
    }
    for (size_t i = 0; i < long_attributes_.size(); ++i) {
      serialized_object["attributes"][double_attributes_.size() + i] = web::json::value(long_attributes_[i].name());
    }

    serialized_object["nodeIds"] = web::json::value::array(connected_nodes_.size());
    for (size_t i = 0; i < connected_nodes_.size(); ++i) {
      serialized_object["nodeIds"][i] = connected_nodes_[i];
    }
  }

  return serialized_object;
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

    for (auto& attribute_storage : double_attributes_) {
      attribute_storage.EraseTimestepValues(time_offset);
    }
    for (auto& attribute_storage : long_attributes_) {
      attribute_storage.EraseTimestepValues(time_offset);
    }
  } else {
    time_offset = first_time_index_ > 0 ? first_time_index_ - 1 : simulation_times_.size() - 1;
  }

  return time_offset;
}

}  // namespace insite