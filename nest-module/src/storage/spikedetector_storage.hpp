#ifndef SPIKE_DETECTOR_STORAGE_HPP
#define SPIKE_DETECTOR_STORAGE_HPP

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace insite {

struct Spike {
  double simulation_time;
  std::uint64_t neuron_id;
};
static_assert(sizeof(Spike) == 2 * 8);

class SpikedetectorStorage {
 public:
  SpikedetectorStorage(std::uint64_t spikedetector_id,
                       std::uint64_t ring_buffer_size = 1000000);

  // Disallow copying
  SpikedetectorStorage(const SpikedetectorStorage&) = delete;
  SpikedetectorStorage& operator=(const SpikedetectorStorage&) = delete;

  void AddSpike(double simulation_time, std::uint64_t neuron_id);
  void ExtractSpikes(std::vector<Spike>* spikes_vector, double from = 0.0,
                     double to = std::numeric_limits<double>::infinity());

 private:
  std::uint64_t id_;
  std::vector<std::uint64_t> neuron_ids_;
  std::vector<Spike> spikes_;
  std::uint64_t first_spike_index_ = 0;
  std::mutex mutex_;
};

}  // namespace insite

#endif