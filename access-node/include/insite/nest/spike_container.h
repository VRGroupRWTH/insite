#pragma once
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <schema_generated.h>
#include <tl/optional.hpp>
#include <vector>

namespace flatbuffers {
class FlatBufferBuilder;
}

namespace insite {

struct Spike {
  double time;
  uint64_t node_id;
  Spike() = default;
  Spike(double time, uint64_t node_id) : time(time), node_id(node_id) {}

  bool CompareByTime(const Spike &other) { return time < other.time; }

  static bool CompareByTime(const Spike &spike_a, const Spike &spike_b) {
    return spike_a.time < spike_b.time;
  }

  bool CompareById(const Spike &other) { return node_id < other.node_id; }

  // compares two given spikes with each other by their nodeId. Returns true if
  // a smaller b
  static bool CompareById(const Spike &spike_a, const Spike &spike_b) {
    return spike_a.node_id < spike_b.node_id;
  }
};

using SpikeVector = std::vector<Spike>;

class SpikeContainer {

public:
  SpikeContainer();
  SpikeContainer(SpikeContainer &&) = default;
  SpikeContainer &operator=(SpikeContainer &&) = default;
  ~SpikeContainer();

  void AddSpikesFromJson(const char *input);
  void AddSpikesFromJsoN2(const char *input);
  void AddSpikesFromJson(const rapidjson::Value &json_doc);

  void AddSpikesFromFlatbuffer(const char *buffer_pointer);

  void SerializeToJson(rapidjson::Writer<rapidjson::StringBuffer> &writer,
                       bool last_frame);

  void SerializeToJson(rapidjson::Writer<rapidjson::StringBuffer> &writer,
                       SpikeVector::const_iterator begin, bool last_frame);

  void SerializeToJson(rapidjson::Writer<rapidjson::StringBuffer> &writer,
                       SpikeVector::const_iterator begin,
                       SpikeVector::const_iterator end, bool last_frame);

  void SerializeToJsonV2(rapidjson::Writer<rapidjson::StringBuffer> &writer,
                         tl::optional<int> skip, tl::optional<int> top,
                         tl::optional<bool> reverse_order, bool last_frame,
                         const std::string &sim_id);

  void SerializeToJson(rapidjson::Writer<rapidjson::StringBuffer> &writer,
                       tl::optional<int> skip, tl::optional<int> top,
                       tl::optional<bool> reverse_order, bool last_frame);

  void SerializeToFlatbuffer(flatbuffers::FlatBufferBuilder &builder);

  void SortByTime();
  void SortByTimePdq();
  void SortByNodeId();

  std::size_t Size();
  SpikeVector::iterator Begin();
  SpikeVector::iterator Begin(std::optional<uint64_t> skip, std::optional<uint64_t> top);

  SpikeVector::iterator Begin(tl::optional<uint64_t> skip, tl::optional<uint64_t> top);
  SpikeVector::iterator End();
  SpikeVector &GetSpikeVector();

private:
  SpikeVector spikes_;
  std::vector<double> times_;
  std::vector<uint64_t> nodeIds_;
  bool last_frame_;
  std::string sim_id;

  static Insite::Nest::Spikes Pack(const Spike &spike);

  static Spike UnPack(const Insite::Nest::Spikes &spike);
};

}; // namespace insite