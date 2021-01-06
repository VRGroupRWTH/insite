#ifndef MULTIMETER_STORAGE_HPP
#define MULTIMETER_STORAGE_HPP

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <cpprest/json.h>
#include "nest_types.h"
#include "node_collection.h"

namespace insite {

template <typename T>
class AttributeStorage {
 public:
  inline AttributeStorage(const std::string& name, std::uint64_t timestep_ring_buffer_size)
      : name_(name), timestep_ring_buffer_size_(timestep_ring_buffer_size), node_count_(0) {}

  inline void SetNodeCount(std::uint64_t node_count) {
    node_count_ = node_count;
    values_.clear();
    values_.resize(timestep_ring_buffer_size_ * node_count_, (T)0);
  }

  inline void EraseTimestepValues(std::uint64_t time_offset) {
    for (size_t i = time_offset * node_count_, end = (time_offset + 1) * node_count_; i < end; ++i) {
      values_[i] = (T)0;
    }
  }

  inline void AddMeasurement(std::uint64_t time_offset, std::uint64_t node_offset, T value) {
    assert(time_offset < timestep_ring_buffer_size_);
    assert(time_offset < timestep_ring_buffer_size_);
  }

  inline std::string name() const { return name_; }

 private:
  std::string name_;
  uint64_t timestep_ring_buffer_size_;
  uint64_t node_count_;
  std::vector<T> values_;
};

class MultimeterStorage {
 public:
  MultimeterStorage(std::uint64_t spikedetector_id,
                    std::uint64_t timestep_ring_buffer_size = 100);

  // Disallow copying
  MultimeterStorage(const MultimeterStorage&) = delete;
  MultimeterStorage& operator=(const MultimeterStorage&) = delete;

  void SetAttributes(const std::vector<Name>& double_attributes,
                     const std::vector<Name>& long_attributes);
  void Prepare(const nest::NodeCollectionPTR& all_nodes);
  void ExtractConnectedNodeIds(std::vector<std::uint64_t>* node_ids);
  void AddMeasurement(double simulation_time, std::uint64_t node_id,
                      const std::vector<double>& double_values,
                      const std::vector<long>& long_values);

  web::json::value Serialize() const;

 private:
  std::uint64_t id_;

  std::vector<double> simulation_times_;
  std::uint64_t first_time_index_ = 0;

  std::vector<std::uint64_t> connected_nodes_;

  std::vector<AttributeStorage<double>> double_attributes_;
  std::vector<AttributeStorage<long>> long_attributes_;
  mutable std::mutex mutex_;

  double GetLatestSimulationTimeNoLock() const;
  uint64_t GetTimeOffsetNoLock(double simulation_time);
};

}  // namespace insite

#endif