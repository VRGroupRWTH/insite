#include "spikedetector_storage.hpp"

namespace insite {

SpikedetectorStorage::SpikedetectorStorage(std::uint64_t spikedetector_id,
                                           std::uint64_t ring_buffer_size)
    : id_(spikedetector_id) {
  spikes_.reserve(ring_buffer_size);
}

void SpikedetectorStorage::AddSpike(double simulation_time,
                                    std::uint64_t neuron_id) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (spikes_.size() < spikes_.capacity()) {
    spikes_.push_back({simulation_time, neuron_id});
  } else {
    spikes_[first_spike_index_].simulation_time = simulation_time;
    spikes_[first_spike_index_].neuron_id = neuron_id;
    ++first_spike_index_;
    if (first_spike_index_ == spikes_.size()) {
      first_spike_index_ = 0;
    }
  }
}

void SpikedetectorStorage::ExtractSpikes(std::vector<Spike>* spikes_vector,
                                         double from, double to) {
  spikes_vector->clear();
  if (spikes_.size() == 0) {
      return;
  }

  std::uint64_t current_index = first_spike_index_;
  const std::uint64_t end_index = first_spike_index_;

  do {
    const auto& current_spike = spikes_[current_index];

    // Todo: make this more efficient by doing a binary search on the simulation
    // time
    if (current_spike.simulation_time >= from &&
        current_spike.simulation_time < to) {
      spikes_vector->push_back(current_spike);
    }

    ++current_index;
    if (current_index == spikes_.size()) {
        current_index = 0;
    }
  } while (current_index != end_index);
}

}  // namespace insite