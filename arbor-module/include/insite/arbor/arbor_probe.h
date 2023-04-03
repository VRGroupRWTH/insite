#pragma once

#include <insite/arbor/optionals_util.h>
#include <insite/arbor/parameters.h>
#include <insite/arbor/pipeline_backend.h>
#include <arbor/cable_cell.hpp>
#include <arbor/common_types.hpp>
#include <arbor/lif_cell.hpp>
#include <arbor/morph/primitives.hpp>
#include <arbor/sampling.hpp>
#include <arbor/schedule.hpp>
#include <arbor/spike.hpp>
#include <arbor/util/any_cast.hpp>
#include <arbor/util/any_ptr.hpp>
#include <arbor/util/any_visitor.hpp>
#include <arbor/util/unique_any.hpp>
#include <tl/optional.hpp>

namespace insite {

union ProbeHashType {
  std::uint64_t hash;
  struct {
    int gid : 32;
    int lid : 16;
    int pid : 16;
  };
};

inline ProbeHashType ProbeHash(int cell_idx, int probe_idx, int meta_index) {
  ProbeHashType a;
  a.gid = cell_idx;
  a.lid = probe_idx;
  a.pid = meta_index;
  return a;
}

inline bool ProbeHasIdTriple(insite::ProbeHashType& probe,
                             tl::optional<int> gid,
                             tl::optional<int> lid,
                             tl::optional<int> pid) {
  bool hasGid = gid.has_value() ? gid.value() == probe.gid : true;
  bool hasLid = lid.has_value() ? lid.value() == probe.lid : true;
  bool hasPid = pid.has_value() ? pid.value() == probe.pid : true;

  return (hasGid && hasLid && hasPid);
}

inline bool IsScalarProbe(std::any probe) {
  return (probe.type() == typeid(arb::cable_probe_membrane_voltage) ||
          probe.type() == typeid(arb::cable_probe_axial_current) ||
          probe.type() == typeid(arb::cable_probe_total_ion_current_density) ||
          probe.type() == typeid(arb::cable_probe_ion_current_density) ||
          probe.type() == typeid(arb::cable_probe_ion_int_concentration) ||
          probe.type() == typeid(arb::cable_probe_ion_ext_concentration) ||
          probe.type() == typeid(arb::cable_probe_point_state));
}

bool ProbeFulfillsProbefilter(ProbeHashType& probe, ProbeParameter& params);

inline bool ProbeHasGid(ProbeHashType& probe, tl::optional<int> gid) {
  return gid.has_value() ? gid.value() == probe.gid : true;
}

std::string ProbeKindToString(std::any probe);
}  // namespace insite
