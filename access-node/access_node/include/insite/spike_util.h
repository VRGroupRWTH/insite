#pragma once
#include <schema_generated.h>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "spdlog/spdlog.h"

namespace insite {

void FlatbufferSpikeTableToJsonSpikeArray(
    const Insite::Nest::SpikeTable* spike_table,
    rapidjson::Writer<rapidjson::StringBuffer>& writer,
    int max_decimal_places = 2) {
  writer.SetMaxDecimalPlaces(max_decimal_places);
  writer.StartArray();

  const auto* spikes = spike_table->spikes();
  for (flatbuffers::uoffset_t i = 0; i < spikes->size(); i++) {
    writer.StartArray();
    writer.Double(spikes->Get(i)->spike_time());
    writer.Uint64(spikes->Get(i)->node_id());
    writer.EndArray();
    SPDLOG_DEBUG(spikes->Get(i)->spike_time());
    SPDLOG_DEBUG(spikes->Get(i)->node_id());
  }

  writer.EndArray();
}
}  // namespace insite