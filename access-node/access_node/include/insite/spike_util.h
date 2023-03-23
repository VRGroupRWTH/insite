#pragma once
#include <schema_generated.h>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "spdlog/spdlog.h"

namespace insite {

void FlatbufferSpikeTableToJsonSpikeArray(const Insite::Nest::SpikeTable* spike_table, rapidjson::Writer<rapidjson::StringBuffer>& writer, int max_decimal_places = 2);

}  // namespace insite
