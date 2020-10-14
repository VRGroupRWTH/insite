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
  std::uint64_t node_count;
  std::string model_name;
  std::vector<std::string> model_parameters;
};

class DataStorage {
 public:
  DataStorage();

  void SetNodesFromCollection(const nest::NodeCollectionPTR& node_collection);
  inline size_t GetNodeCollectionCount() const {
    std::unique_lock<std::mutex> lock(node_collections_mutex_);
    return node_collections_.size();
  }
  inline NodeCollection GetNodeCollection(size_t id) const {
    std::unique_lock<std::mutex> lock(node_collections_mutex_);
    return node_collections_[id];
  }
  inline std::vector<NodeCollection> GetNodeCollections() const {
    std::unique_lock<std::mutex> lock(node_collections_mutex_);
    return node_collections_;
  }

  std::shared_ptr<SpikedetectorStorage> CreateSpikeDetectorStorage(std::uint64_t spike_detector_id);
  std::shared_ptr<SpikedetectorStorage> GetSpikeDetectorStorage(std::uint64_t spike_detector_id);
  std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> GetSpikeDetectors() const {
    std::unique_lock<std::mutex> lock(spikedetectors_mutex_);
    return spikedetectors_;
  }

  void AddSpike(std::uint64_t spikedetector_id, double simulation_time, std::uint64_t neuron_id);

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
  mutable std::mutex node_collections_mutex_;
  std::vector<NodeCollection> node_collections_;

  std::atomic_uint64_t current_simulation_time_;
  std::atomic_uint64_t simulation_begin_time_;
  std::atomic_uint64_t simulation_end_time_;

  
  mutable std::mutex spikedetectors_mutex_;
  std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spikedetectors_;

  // Device ID to attribute index to measurement map.
  std::unordered_map<std::uint64_t, std::unordered_map<std::string, 
    MultimeterMeasurements>> buffered_measurements_;

  std::mutex measurement_mutex_;
};

}  // namespace insite

#endif