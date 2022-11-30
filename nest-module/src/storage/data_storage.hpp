#ifndef DATA_STORATE_HPP
#define DATA_STORATE_HPP

#include <cpprest/json.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "../serialize.hpp"
#include "dictdatum.h"
#include "model.h"
#include "multimeter_storage.hpp"
#include "node_collection.h"
#include "spikedetector_storage.hpp"

namespace insite {

struct Node {
  uint64_t node_id;
  uint64_t node_collection_id;
  uint64_t simulation_node_id;
  std::vector<double> position;
  std::string model_name;
  DictionaryDatum status;
};

struct NodeCollection {
  std::uint64_t first_node_id;
  std::uint64_t node_count;
  std::string model_name;
  // web::json::value model_status;
  std::string model_status;

  bool operator<(const NodeCollection& c2) const {
    return this->first_node_id < c2.first_node_id;
  }

  bool operator==(const NodeCollection& c2) const {
    return this->first_node_id == c2.first_node_id && this->node_count == c2.node_count && this->model_name == c2.model_name;
  }
};

std::ostream& operator<<(std::ostream& os, const NodeCollection& c);

class DataStorage {
 public:
  DataStorage();

  void SetNodesFromCollection(const nest::NodeCollectionPTR& local_node_collection);
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
  inline std::unordered_map<uint64_t, std::string> GetNodes() const {
    std::unique_lock<std::mutex> lock(node_collections_mutex_);
    return nodes_;
  }
  uint64_t GetNodeCollectionIdForNodeId(uint64_t node_id) const;

  std::shared_ptr<SpikedetectorStorage> CreateSpikeDetectorStorage(std::uint64_t spike_detector_id);
  std::shared_ptr<SpikedetectorStorage> GetSpikeDetectorStorage(std::uint64_t spike_detector_id);
  std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> GetSpikeDetectors() const {
    std::unique_lock<std::mutex> lock(spikedetectors_mutex_);
    return spikedetectors_;
  }

  std::shared_ptr<MultimeterStorage> CreateMultimeterStorage(std::uint64_t multimeter_id);
  std::shared_ptr<MultimeterStorage> GetMultimeterStorage(std::uint64_t multimeter_id);
  std::unordered_map<std::uint64_t, std::shared_ptr<MultimeterStorage>> GetMultimeters() const {
    std::unique_lock<std::mutex> lock(multimeters_mutex_);
    return multimeters_;
  }

  void AddSpike(std::uint64_t spikedetector_id, double simulation_time, std::uint64_t node_id);
  void AddMultimeterMeasurement(std::uint64_t multimeter_id, double simulation_time, std::uint64_t node_id, const std::vector<double>& double_values, const std::vector<long>& long_values);

  void SetCurrentSimulationTime(double simulation_time);
  void SetSimulationTimeRange(double begin, double end);
  double GetCurrentSimulationTime() const;
  double GetSimulationBeginTime() const;
  double GetSimulationEndTime() const;

  // inline void SetKernelStatus(const web::json::value& kernel_status) {
  //   std::unique_lock<std::mutex> lock(kernel_status_mutex_);
  //   kernel_status_ = kernel_status;
  // }

  inline void SetDictKernelStatus(const DictionaryDatum& kernel_status) {
    std::unique_lock<std::mutex> lock(kernel_status_mutex_);
    dict_kernel_status_ = kernel_status;
  }

  // inline web::json::value GetKernelStatus() const {
  //   std::unique_lock<std::mutex> lock(kernel_status_mutex_);
  //   return kernel_status_;
  // }

  inline void GetKernelStatus(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
    std::unique_lock<std::mutex> lock(kernel_status_mutex_);
    SerializeDatum(dict_kernel_status_, writer);
  }
  void Reset();

 private:
  void SerializeNode(rapidjson::Writer<rapidjson::StringBuffer>& writer, const nest::NodeIDTriple& node_id_triple) const;
  uint64_t find_node_collection_index_for_node_id(const uint64_t node_id) const;

  uint64_t GetNodeCollectionIdForNodeIdNoLock(uint64_t node_id) const;

  std::vector<NodeCollection> ReceiveCollectionsFromNode(int source);
  mutable std::mutex node_collections_mutex_;
  std::vector<NodeCollection> node_collections_;
  // std::unordered_map<uint64_t, web::json::value> nodes_;
  std::unordered_map<uint64_t, std::string> nodes_;

  std::atomic_uint64_t current_simulation_time_;
  std::atomic_uint64_t simulation_begin_time_;
  std::atomic_uint64_t simulation_end_time_;

  mutable std::mutex kernel_status_mutex_;
  // web::json::value kernel_status_;
  DictionaryDatum dict_kernel_status_;

  mutable std::mutex spikedetectors_mutex_;
  std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spikedetectors_;

  mutable std::mutex multimeters_mutex_;
  std::unordered_map<std::uint64_t, std::shared_ptr<MultimeterStorage>> multimeters_;
};

}  // namespace insite

#endif
