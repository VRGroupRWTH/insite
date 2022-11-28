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
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
// #include "../rapidjson/prettywriter.h"

namespace insite {

class AttributeStorageBase {
 public:
  virtual ~AttributeStorageBase() = default;

  virtual void SetNodeCount(std::uint64_t node_count) = 0;
  virtual void EraseTimestepValues(std::uint64_t time_offset) = 0;

  virtual std::string GetName() const = 0;
  virtual void ExtractValues(rapidjson::Writer<rapidjson::StringBuffer> &writer ,const std::vector<uint64_t>& time_indices, const std::vector<uint64_t>& node_indices) const = 0;
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

  inline void ExtractValues(rapidjson::Writer<rapidjson::StringBuffer> &writer, const std::vector<uint64_t>& time_indices, const std::vector<uint64_t>& node_indices) const override {
    static_assert(std::is_same<T,long>::value || std::is_same<T,double>::value, "Attribute storage must be long or double");
    web::json::value values = web::json::value::array(time_indices.size() * node_indices.size());
    writer.StartArray();
    for (uint64_t t = 0; t < time_indices.size(); ++t) {
      const uint64_t time_index = time_indices[t];
      assert(time_index < timestep_ring_buffer_size_);

      for (uint64_t n = 0; n < node_indices.size(); ++n) {
        const uint64_t node_index = node_indices[n];
        assert(node_index < node_count_);
        auto array_offset = time_index * node_count_ + node_index;
        if(std::is_same<T,long>::value)
            writer.Int(values_[array_offset]);
        if(std::is_same<T,double>::value)
            writer.Double(values_[array_offset]);
      }
    }
    writer.EndArray();
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
                    std::uint64_t timestep_ring_buffer_size = 1000000);

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

  void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;

  void ExtractMeasurements(rapidjson::Writer<rapidjson::StringBuffer> &writer,
      const std::string& attribute_name, const std::vector<uint64_t>& node_ids = {},
      double from_time = 0.0, double to_time = std::numeric_limits<double>::infinity());
  std::uint64_t id_;
 private:

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
