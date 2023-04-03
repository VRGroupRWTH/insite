#pragma once
#include <insite/arbor/optionals_util.h>
#include <insite/arbor/pipeline_backend.h>

namespace insite {
inline bool SpikeFulfillsSpikefilter(
    const arb::basic_spike<arb::cell_member_type>& spike,
    const insite::SpikeParameter& filter_parameter) {
  return ((filter_parameter.g_id_.empty() ||
           ValueInList<int>(filter_parameter.g_id_,
                            static_cast<int>(spike.source.gid))) &&
          ValueInOptionalRange(spike.time, filter_parameter.fromTime_,
                               filter_parameter.toTime_));
}
}  // namespace insite
