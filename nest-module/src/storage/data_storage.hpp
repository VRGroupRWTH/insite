#ifndef DATA_STORATE_HPP
#define DATA_STORATE_HPP

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <atomic>

#include "spikedetector_storage.hpp"
#include "node_collection.h"

namespace insite {
struct MultimeterInfo {
  std::uint64_t device_id;
  bool needs_update;
  std::vector<std::string> double_attributes;
  std::vector<std::string> long_attributes;
  std::vector<std::uint64_t> gids;
};

struct MultimeterMeasurements {
  std::vector<double> simulation_times;
  std::vector<std::uint64_t> gids;
  std::vector<double> values;
};

struct NodeCollection {
  std::uint64_t first_node_id;
  std::uint64_t last_node_id;
};

class DataStorage {
 public:
  DataStorage();

  void SetNodesFromCollection(const nest::NodeCollectionPTR& node_collection);

  SpikedetectorStorage* GetSpikeDetectorStorage(std::uint64_t spike_detector_id);
  void AddSpike(std::uint64_t spikedetector_id, double simulation_time, std::uint64_t neuron_id);

  std::vector<Spike> GetSpikes();

  void AddMultimeterMeasurement(std::uint64_t device_id, 
    const std::string& attribute_name, const double simulation_time,
    const std::uint64_t gid, const double value);
  std::unordered_map<std::uint64_t, std::unordered_map<std::string, 
    MultimeterMeasurements>> GetMultimeterMeasurements();

  void SetCurrentSimulationTime(double simulation_time);
  void SetSimulationTimeRange(double begin, double end);
  double GetCurrentSimulationTime() const;
  double GetSimulationBeginTime() const;
  double GetSimulationEndTime() const;

 private:
  std::mutex neuron_ids_mutex_;
  std::vector<NodeCollection> node_collections_;

  std::atomic_uint64_t current_simulation_time_;
  std::atomic_uint64_t simulation_begin_time_;
  std::atomic_uint64_t simulation_end_time_;

  
  std::unordered_map<std::uint64_t, std::unique_ptr<SpikedetectorStorage>> spikedetectors_;
  std::mutex spikedetectors_mutex_;

  // Device ID to attribute index to measurement map.
  std::unordered_map<std::uint64_t, std::unordered_map<std::string, 
    MultimeterMeasurements>> buffered_measurements_;

  std::mutex measurement_mutex_;
};

}  // namespace insite

#endif