#include "spikedetector_storage.hpp"
#include <algorithm>
#include <iterator>

#include "kernel_manager.h"

namespace insite {

SpikedetectorStorage::SpikedetectorStorage(std::uint64_t spikedetector_id,
                                           std::uint64_t ring_buffer_size)
    : id_(spikedetector_id) {
  spikes_.reserve(ring_buffer_size);
  spikes32_.reserve(ring_buffer_size);
}

SpikedetectorStorage::~SpikedetectorStorage() {
}

void SpikedetectorStorage::Prepare(const nest::NodeCollectionPTR& all_nodes) {
  std::unique_lock<std::mutex> lock(mutex_);
  nest::NodeCollectionPTR spike_detector_collection =
      std::make_shared<nest::NodeCollectionPrimitive>(id_, id_);

  std::deque<nest::ConnectionID> connections;
  for (nest::synindex synapse_id = 0;
       synapse_id < nest::kernel().model_manager.get_num_connection_models();
       ++synapse_id) {
    nest::kernel().connection_manager.get_connections(
        connections, all_nodes, spike_detector_collection, synapse_id, -1);
  }

  connected_nodes_.resize(0);
  connected_nodes_.reserve(connections.size());
  for (const auto& connection : connections) {
    connected_nodes_.push_back(connection.get_source_node_id());
  }
}

void SpikedetectorStorage::ExtractConnectedNodeIds(std::vector<std::uint64_t>* node_ids) {
  assert(node_ids != nullptr);
  std::unique_lock<std::mutex> lock(mutex_);

  *node_ids = connected_nodes_;
}

void SpikedetectorStorage::AddSpike(double simulation_time,
                                    std::uint64_t node_id) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (spikes_.size() < spikes_.capacity()) {
    spikes_.push_back({simulation_time, node_id});
    spikes32_.push_back({static_cast<float>(simulation_time), static_cast<uint32_t>(node_id)});
  } else {
    spikes_[first_spike_index_].simulation_time = simulation_time;
    spikes_[first_spike_index_].node_id = node_id;
    spikes32_[first_spike_index_].simulation_time = simulation_time;
    spikes32_[first_spike_index_].node_id = static_cast<uint32_t>(node_id);
    ++first_spike_index_;
    if (first_spike_index_ == spikes_.size()) {
      first_spike_index_ = 0;
    }
  }
}

void SpikedetectorStorage::ExtractSpikes3(std::vector<Spike>* spikes_vector,
                                          double from_time,
                                          double to_time,
                                          std::uint64_t from_neuron_id,
                                          std::uint64_t to_neuron_id,
                                          std::vector<std::uint64_t>* node_ids) {
  if (spikes_.size() == 0) {
    return;
  }

  std::copy(spikes_.begin(), spikes_.end(), std::back_inserter(*spikes_vector));
}

void SpikedetectorStorage::ExtractSpikes2(std::vector<Spike>* spikes_vector,
                                          double from_time,
                                          double to_time,
                                          std::uint64_t from_neuron_id,
                                          std::uint64_t to_neuron_id,
                                          std::vector<std::uint64_t>* node_ids) {
  if (spikes_.size() == 0) {
    return;
  }

  std::copy_if(spikes_.begin(), spikes_.end(), std::back_inserter(*spikes_vector), [&](Spike& spike) { return (spike.simulation_time >= from_time &&
                                                                                                               spike.simulation_time < to_time &&
                                                                                                               spike.node_id >= from_neuron_id &&
                                                                                                               spike.node_id <= to_neuron_id); });
}

void SpikedetectorStorage::ExtractSpikesInt32(std::vector<Spike32>* spikes_vector,
                                              double from_time,
                                              double to_time,
                                              std::uint64_t from_neuron_id,
                                              std::uint64_t to_neuron_id,
                                              std::vector<std::uint64_t>* node_ids) {
  if (spikes32_.size() == 0) {
    return;
  }

  std::copy_if(spikes32_.begin(), spikes32_.end(), std::back_inserter(*spikes_vector), [&](Spike32& spike) { return (spike.simulation_time >= from_time &&
                                                                                                                     spike.simulation_time < to_time &&
                                                                                                                     spike.node_id >= from_neuron_id &&
                                                                                                                     spike.node_id <= to_neuron_id); });
}

void SpikedetectorStorage::ExtractSpikes(std::vector<Spike>* spikes_vector,
                                         double from_time,
                                         double to_time,
                                         std::uint64_t from_neuron_id,
                                         std::uint64_t to_neuron_id,
                                         std::vector<std::uint64_t>* node_ids) {
  if (spikes_.size() == 0) {
    return;
  }

  std::uint64_t current_index = first_spike_index_;
  const std::uint64_t end_index = first_spike_index_;

  do {
    const auto& current_spike = spikes_[current_index];

    // Todo: make this more efficient by doing a binary search on the simulation
    // time
    if (current_spike.simulation_time >= from_time &&
        current_spike.simulation_time < to_time &&
        current_spike.node_id >= from_neuron_id &&
        current_spike.node_id <= to_neuron_id) {
      if (node_ids->empty() || std::find(node_ids->begin(), node_ids->end(), current_spike.node_id) != node_ids->end())
        spikes_vector->push_back(current_spike);
    }

    ++current_index;
    if (current_index == spikes_.size()) {
      current_index = 0;
    }
  } while (current_index != end_index);
}

void SpikedetectorStorage::ExtractSpikes(rapidjson::Writer<rapidjson::StringBuffer>& writer,
                                         double from_time,
                                         double to_time,
                                         std::uint64_t from_neuron_id,
                                         std::uint64_t to_neuron_id,
                                         std::vector<std::uint64_t>* node_ids) {
  if (spikes_.size() == 0) {
    return;
  }

  std::uint64_t current_index = first_spike_index_;
  const std::uint64_t end_index = first_spike_index_;

  // writer.StartObject();
  // writer.Key("spikes");
  // writer.StartArray();
  int i = 0;
  do {
    const auto& current_spike = spikes_[current_index];
    // Todo: make this more efficient by doing a binary search on the simulation
    // time
    // writer.StartArray();
    // writer.Double(current_spike.simulation_time);
    // writer.Int(current_spike.node_id);
    // writer.EndArray();
    // spikes_vector->push_back(current_spike);

    if (current_spike.simulation_time >= from_time &&
        current_spike.simulation_time < to_time &&
        current_spike.node_id >= from_neuron_id &&
        current_spike.node_id <= to_neuron_id) {
      i++;
    }
    ++current_index;
    if (current_index == spikes_.size()) {
      current_index = 0;
    }
  } while (current_index != end_index);
  std::cout << i << std::endl;

  // writer.EndArray();
  // writer.EndArray();
  // writer.Key("lastFrame");
  // writer.Bool(false);
  // writer.EndObject();
}

}  // namespace insite
