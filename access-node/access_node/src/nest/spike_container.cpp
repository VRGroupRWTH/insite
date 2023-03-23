#include <nest/spike_container.h>
#include "flatbuffers/base.h"
#include "flatbuffers/flatbuffer_builder.h"
#include "flatbuffers/vector.h"
#include "pdqsort.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <exception>
#include <iterator>
#include <nest/json_strings.h>
#include <nest/spike_container.h>
#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <tl/optional.hpp>

namespace insite {

SpikeContainer::SpikeContainer() {}

SpikeContainer::~SpikeContainer() {}

void SpikeContainer::AddSpikesFromJson(const char *input) {
  rapidjson::Document document;
  document.Parse(input);

  const rapidjson::Value &times = document[json_strings::kSimTimes];
  const rapidjson::Value &node_ids = document[json_strings::kNodeIds];
  last_frame_ = document[json_strings::kLastFrame].GetBool();

  for (std::uint64_t i = 0; i < times.Size(); ++i) {
    spikes_.emplace_back(times[i].GetDouble(), node_ids[i].GetUint64());
  }
  SPDLOG_DEBUG("[SpikeContainer::addSpikesFromJSON] Added {} spikes to "
               "SpikeContainer for a total size of {}",
               times.Size(), spikes_.size());
}

void SpikeContainer::AddSpikesFromJson(const rapidjson::Value &json_doc) {
  last_frame_ = json_doc[json_strings::kLastFrame].GetBool();
  for (int i = 0; i < json_doc[json_strings::kSimTimes].GetArray().Size();
       ++i) {
    double sim_time =
        json_doc[json_strings::kSimTimes].GetArray()[i].GetDouble();
    uint64_t node_id =
        json_doc[json_strings::kNodeIds].GetArray()[i].GetUint64();
    spikes_.emplace_back(sim_time, node_id);
  }
  SPDLOG_DEBUG("[SpikeContainer::addSpikesFromJSON] Added {} spikes to "
               "SpikeContainer for a total size of {}",
               json_doc[json_strings::kSimTimes].GetArray().Size(),
               spikes_.size());
}

void SpikeContainer::AddSpikesFromFlatbuffer(const char *buffer_pointer) {
  const auto *spikes = Insite::Nest::GetSpikeTable(buffer_pointer)->spikes();
  // spikes_.resize(spikes->size());
  for (flatbuffers::uoffset_t i = 0; i < spikes->size(); i++) {
    // spikes_[i] = UnPack(*spikes->Get(i));
    spikes_.emplace_back(UnPack(*spikes->Get(i)));
  }
  SPDLOG_DEBUG(
      "[SpikeContainer::addSpikesFromFlatbuffer] Added {} spikes for a "
      "total size of {}",
      spikes->size(), spikes_.size());
}

void SpikeContainer::SerializeToJson(
    rapidjson::Writer<rapidjson::StringBuffer> &writer, bool last_frame) {
  writer.StartObject();
  SerializeToJson(writer, Begin(), End(), last_frame);
  writer.EndObject();
}

void SpikeContainer::SerializeToJson(
    rapidjson::Writer<rapidjson::StringBuffer> &writer,
    SpikeVector::const_iterator begin, bool last_frame) {
  writer.StartObject();
  SerializeToJson(writer, begin, End(), last_frame);
  writer.EndObject();
}

void SpikeContainer::SerializeToJsonV2(
    rapidjson::Writer<rapidjson::StringBuffer> &writer, tl::optional<int> skip,
    tl::optional<int> top, tl::optional<bool> reverse_order, bool last_frame,
    const std::string &sim_id) {

  writer.StartObject();
  writer.Key("simId");
  writer.String(sim_id.c_str());
  SerializeToJson(writer, skip, top, reverse_order, last_frame_);
  writer.EndObject();
}
void SpikeContainer::SerializeToJson(
    rapidjson::Writer<rapidjson::StringBuffer> &writer, tl::optional<int> skip,
    tl::optional<int> top, tl::optional<bool> reverse_order, bool last_frame) {
  SPDLOG_DEBUG("[SpikeContainer::serializeToJSON] serializing {} spikes",
               spikes_.size());
  writer.SetMaxDecimalPlaces(14);
  SpikeVector::iterator end;
  SpikeVector::iterator begin;
  const std::size_t number_of_spikes = Size();

  if (!reverse_order || reverse_order == false) {
    begin = Begin();
    if (skip) {
      const std::size_t skip_value = skip.value();
      auto start_offset =
          std::clamp(skip_value, static_cast<std::size_t>(0), number_of_spikes);
      std::advance(begin, start_offset);
    }

    if (top) {
      const std::size_t skip_value = skip.value_or(0);
      const std::size_t top_value = top.value();
      auto end_offset =
          std::clamp(skip_value + top_value, static_cast<std::size_t>(0),
                     number_of_spikes);

      end = Begin();
      std::advance(end, end_offset);
    } else {
      end = End();
    }

  } else if (reverse_order == true) {
    end = End();
    if (skip) {
      const std::size_t skip_value = skip.value();
      auto start_offset =
          std::clamp(skip_value, static_cast<std::size_t>(0), number_of_spikes);
      std::advance(end, -start_offset);
    }

    if (top) {
      begin = End();

      const std::size_t skip_value = skip.value_or(0);
      const std::size_t top_value = top.value();
      auto end_offset =
          std::clamp(skip_value + top_value, static_cast<std::size_t>(0),
                     number_of_spikes);
      std::advance(begin, -end_offset);
    } else {
      begin = Begin();
    }
  }

  writer.Key("simulationTimes");
  writer.StartArray();
  for (auto it = begin; it != end; it++) {
    writer.Double(it->time);
  }
  writer.EndArray();

  writer.Key("nodeIds");
  writer.StartArray();
  for (auto it = begin; it != end; it++) {
    writer.Uint64(it->node_id);
  }
  writer.EndArray();
  writer.Key("lastFrame");
  if (end != End()) {
    writer.Bool(false);
  } else {
    writer.Bool(last_frame_);
  }
}

void SpikeContainer::SerializeToJson(
    rapidjson::Writer<rapidjson::StringBuffer> &writer,
    SpikeVector::const_iterator begin, SpikeVector::const_iterator end,
    bool last_frame) {
  SPDLOG_DEBUG("[SpikeContainer::serializeToJSON] serializing {} spikes",
               spikes_.size());
  writer.SetMaxDecimalPlaces(14);

  writer.Key("simulationTimes");
  writer.StartArray();
  for (auto it = begin; it != end; it++) {
    writer.Double(it->time);
  }
  writer.EndArray();

  writer.Key("nodeIds");
  writer.StartArray();
  for (auto it = begin; it != end; it++) {
    writer.Uint64(it->node_id);
  }
  writer.EndArray();
  writer.Key("lastFrame");
  writer.Bool(last_frame_);
}

void SpikeContainer::SerializeToFlatbuffer(
    flatbuffers::FlatBufferBuilder &builder) {
  SPDLOG_DEBUG("[SpikeContainer::serializeToFlatbuffer] serializing {} spikes",
               spikes_.size());
  auto fb_spikes = builder.CreateVectorOfNativeStructs<Insite::Nest::Spikes>(
      spikes_, SpikeContainer::Pack);
  auto fb_spike_table = Insite::Nest::CreateSpikeTable(builder, fb_spikes);
  builder.Finish(fb_spike_table);
}

void SpikeContainer::SortByTime() {
  SPDLOG_DEBUG("[SpikeContainer::sortByTime] called");
  std::sort(spikes_.begin(), spikes_.end(),
            static_cast<bool (*)(const Spike &, const Spike &)>(
                Spike::CompareByTime));
}

void SpikeContainer::SortByTimePdq() {
  SPDLOG_DEBUG("[SpikeContainer::sortByTimePdq] called");
  pdqsort_branchless(spikes_.begin(), spikes_.end(),
                     static_cast<bool (*)(const Spike &, const Spike &)>(
                         Spike::CompareByTime));
}

void SpikeContainer::SortByNodeId() {
  SPDLOG_DEBUG("[SpikeContainer::sortByNodeId] called");
  std::sort(
      spikes_.begin(), spikes_.end(),
      static_cast<bool (*)(const Spike &, const Spike &)>(Spike::CompareById));
}

SpikeVector::iterator SpikeContainer::Begin(tl::optional<uint64_t> skip,
                                            tl::optional<uint64_t> top) {
  std::optional<uint64_t> std_skip;
  std::optional<uint64_t> std_top;

  if (skip) {
    std_skip = skip.value();
  }
  if (top) {
    std_top = top.value();
  }
  return Begin(std_skip, std_top);
}

SpikeVector::iterator SpikeContainer::Begin(std::optional<uint64_t> skip,
                                            std::optional<uint64_t> top) {
  uint64_t start_index = 0;
  auto length = Size();
  if (skip && (skip.value() > length)) {
    skip = length;
  }

  if (top && (top.value() > length)) {
    top = length;
  }

  if (top && (!skip || (skip && (skip.value() < length - top.value())))) {
    start_index = length - top.value();
  } else if (skip &&
             (!top || (top && (skip.value() >= length - top.value())))) {
    start_index = skip.value();
  }

  if (start_index < 0) {
    start_index = 0;
  } else if (start_index > length) {
    start_index = length;
  }
  auto begin = spikes_.begin();
  std::advance(begin, start_index);
  return begin;
}

SpikeVector::iterator SpikeContainer::Begin() { return spikes_.begin(); }

std::size_t SpikeContainer::Size() { return spikes_.size(); };

SpikeVector::iterator SpikeContainer::End() { return spikes_.end(); };

SpikeVector& SpikeContainer::GetSpikeVector() { return spikes_; };

Insite::Nest::Spikes SpikeContainer::Pack(const Spike &spike) {
  return Insite::Nest::Spikes{spike.time, spike.node_id};
};

Spike SpikeContainer::UnPack(const Insite::Nest::Spikes &spike) {
  return Spike{spike.spike_time(), spike.node_id()};
};

} // namespace insite
