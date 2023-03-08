#ifndef SPIKE_DETECTOR_STORAGE_HPP
#define SPIKE_DETECTOR_STORAGE_HPP

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "node_collection.h"

namespace insite {

struct Spike {
  double simulation_time;
  std::uint64_t node_id;
};

struct Spike32 {
  float simulation_time;
  std::uint32_t node_id;
};
static_assert(sizeof(Spike) == 2 * 8, "");

class SpikedetectorStorage {
 public:
  SpikedetectorStorage(std::uint64_t spikedetector_id,
                       std::uint64_t ring_buffer_size = 40000000);

  // Disallow copying
  SpikedetectorStorage(const SpikedetectorStorage&) = delete;
  SpikedetectorStorage& operator=(const SpikedetectorStorage&) = delete;
  ~SpikedetectorStorage();
  void Prepare(const nest::NodeCollectionPTR& all_nodes);
  void ExtractConnectedNodeIds(std::vector<std::uint64_t>* node_ids);

  void AddSpike(double simulation_time, std::uint64_t node_id);
  void ExtractSpikes(
      std::vector<Spike>* spikes_vector,
      double from_time = 0.0,
      double to_time = std::numeric_limits<double>::infinity(),
      std::uint64_t from_neuron_id = 0,
      std::uint64_t to_neuron_id = std::numeric_limits<std::uint64_t>::max(),
      std::vector<std::uint64_t>* node_ids = nullptr);

  void ExtractSpikes3(
      std::vector<Spike>* spikes_vector,
      double from_time = 0.0,
      double to_time = std::numeric_limits<double>::infinity(),
      std::uint64_t from_neuron_id = 0,
      std::uint64_t to_neuron_id = std::numeric_limits<std::uint64_t>::max(),
      std::vector<std::uint64_t>* node_ids = nullptr);

  void ExtractSpikesInt32(
      std::vector<Spike32>* spikes_vector,
      double from_time = 0.0,
      double to_time = std::numeric_limits<double>::infinity(),
      std::uint64_t from_neuron_id = 0,
      std::uint64_t to_neuron_id = std::numeric_limits<std::uint64_t>::max(),
      std::vector<std::uint64_t>* node_ids = nullptr);

  void ExtractSpikes2(
      std::vector<Spike>* spikes_vector,
      double from_time = 0.0,
      double to_time = std::numeric_limits<double>::infinity(),
      std::uint64_t from_neuron_id = 0,
      std::uint64_t to_neuron_id = std::numeric_limits<std::uint64_t>::max(),
      std::vector<std::uint64_t>* node_ids = nullptr);

  void ExtractSpikes(
      rapidjson::Writer<rapidjson::StringBuffer>& writer,
      double from_time,
      double to_time,
      std::uint64_t from_neuron_id,
      std::uint64_t to_neuron_id,
      std::vector<std::uint64_t>* node_ids);

 private:
  std::uint64_t id_;
  std::vector<std::uint64_t> connected_nodes_;
  std::vector<Spike> spikes_;
  std::vector<Spike32> spikes32_;
  std::uint64_t first_spike_index_ = 0;
  std::mutex mutex_;
};

}  // namespace insite

#endif
