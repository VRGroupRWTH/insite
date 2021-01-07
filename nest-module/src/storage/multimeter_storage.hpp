#ifndef MULTIMETER_STORAGE_HPP
#define MULTIMETER_STORAGE_HPP

#include <cpprest/json.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "nest_types.h"
#include "node_collection.h"

namespace insite {

class AttributeStorageBase {
 public:
  virtual ~AttributeStorageBase() = default;

  virtual void SetNodeCount(std::uint64_t node_count) = 0;
  virtual void EraseTimestepValues(std::uint64_t time_offset) = 0;

  virtual std::string GetName() const = 0;
  virtual web::json::value ExtractValues(const std::vector<uint64_t>& time_indices, const std::vector<uint64_t>& node_indices) const = 0;
};

template <typename T>
class AttributeStorage : public AttributeStorageBase {
 public:
  inline AttributeStorage(const std::string& name, std::uint64_t timestep_ring_buffer_size)
      : name_(name), timestep_ring_buffer_size_(timestep_ring_buffer_size), node_count_(0) {}

  // Disallow copying
  AttributeStorage(const AttributeStorage&) = delete;
  AttributeStorage& operator=(const AttributeStorage&) = delete;

  inline void SetNodeCount(std::uint64_t node_count) override {
    node_count_ = node_count;
    values_.clear();
    values_.resize(timestep_ring_buffer_size_ * node_count_, (T)0);
  }

  inline void EraseTimestepValues(std::uint64_t time_offset) override {
    for (size_t i = time_offset * node_count_, end = (time_offset + 1) * node_count_; i < end; ++i) {
      values_[i] = (T)0;
    }
  }

  inline void AddMeasurement(std::uint64_t time_offset, std::uint64_t node_offset, T value) {
    assert(time_offset < timestep_ring_buffer_size_);
    assert(time_offset < timestep_ring_buffer_size_);
    values_[time_offset * node_count_ + node_offset] = value;
  }

  inline std::string GetName() const override { return name_; }

  inline web::json::value ExtractValues(const std::vector<uint64_t>& time_indices, const std::vector<uint64_t>& node_indices) const override {
    web::json::value values = web::json::value::array(time_indices.size() * node_indices.size());
    for (uint64_t t = 0; t < time_indices.size(); ++t) {
      const uint64_t time_index = time_indices[t];
      assert(time_index < timestep_ring_buffer_size_);

      for (uint64_t n = 0; n < node_indices.size(); ++n) {
        const uint64_t node_index = node_indices[n];
        assert(node_index < node_count_);

        values[t * node_indices.size() + n] = values_[time_index * node_count_ + node_index];
      }
    }

    return values;
  }

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
  web::json::value ExtractMeasurements(
      const std::string& attribute_name, const std::vector<uint64_t>& node_ids = {},
      double from_time = 0.0, double to_time = std::numeric_limits<double>::infinity());

 private:
  std::uint64_t id_;

  std::vector<double> simulation_times_;
  std::uint64_t first_time_index_ = 0;

  std::vector<std::uint64_t> connected_nodes_;

  std::vector<std::shared_ptr<AttributeStorage<double>>> double_attributes_;
  std::vector<std::shared_ptr<AttributeStorage<long>>> long_attributes_;
  std::map<std::string, std::weak_ptr<AttributeStorageBase>> attributes_;
  mutable std::mutex mutex_;

  double GetLatestSimulationTimeNoLock() const;
  uint64_t GetTimeOffsetNoLock(double simulation_time);
};

}  // namespace insite

#endif