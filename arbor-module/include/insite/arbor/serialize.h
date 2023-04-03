#pragma once
#include <insite/arbor/pipeline_backend.h>
#include <arbor/cable_cell.hpp>
#include <arbor/common_types.hpp>
#include <arbor/lif_cell.hpp>
#include <arbor/spike.hpp>
#include <arbor/util/unique_any.hpp>
#include <vector>

namespace insite {
std::string CellKindToString(arb::cell_kind cell);
void CellDescriptionAsJSON(StringBufferWriter& writer,
                           const arb::util::unique_any* cell_desc);

void SerializeCell(StringBufferWriter& writer, int gid, arb::cell_kind& cell_kind, arb::util::unique_any* cell_desc);

void SerializeCellInfo(StringBufferWriter& writer, int gid, arb::cell_kind& cell_kind, arb::util::unique_any* cell_desc);

void SerializeProbe(StringBufferWriter& writer, const ProbeInfoInsite& probe);

void SerializeSpikes(
    StringBufferWriter& writer,
    const std::vector<arb::basic_spike<arb::cell_member_type>>& spikes,
    SpikeParameter& params);

void SerializeProbeData(rapidjson::Writer<rapidjson::StringBuffer>& writer,
                        const std::vector<insite::ProbeData>& probe_data,
                        ProbeHashType probe_hash,
                        ProbeParameter& params);
}  // namespace insite
