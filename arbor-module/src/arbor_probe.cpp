#include <insite/arbor/arbor_probe.h>
#include <sstream>

namespace insite {

bool ProbeFulfillsProbefilter(ProbeHashType& probe, ProbeParameter& params) {
  if (!params.hash_.empty() &&
      !ValueInList<std::uint64_t>(params.hash_, probe.hash)) {
    return false;
  }

  if (!ProbeHasIdTriple(probe, params.g_id_, params.l_id_, params.p_id_)) {
    return false;
  }
  return true;
}

std::string ProbeKindToString(std::any probe) {
  if (probe.type() == typeid(arb::cable_probe_membrane_voltage)) {
    auto casted_probe = std::any_cast<arb::cable_probe_membrane_voltage>(probe);
    std::stringstream locs;
    locs << casted_probe.locations;
    return "cable_probe_membrane_voltage";
  }

  if (probe.type() == typeid(arb::cable_probe_membrane_voltage_cell)) {
    return "cable_probe_membrane_voltage_cell";
  }

  if (probe.type() == typeid(arb::cable_probe_axial_current)) {
    auto casted_probe = std::any_cast<arb::cable_probe_axial_current>(probe);
    std::stringstream locs;
    locs << casted_probe.locations;
    return "cable_probe_axial_current";
  }

  if (probe.type() == typeid(arb::cable_probe_total_ion_current_density)) {
    auto casted_probe =
        std::any_cast<arb::cable_probe_total_ion_current_density>(probe);
    std::stringstream locs;
    locs << casted_probe.locations;
    return "cable_probe_total_ion_current_density";
  }

  if (probe.type() == typeid(arb::cable_probe_total_ion_current_cell)) {
    auto casted_probe =
        std::any_cast<arb::cable_probe_total_ion_current_cell>(probe);
    return "cable_probe_total_ion_current_cell";
  }

  if (probe.type() == typeid(arb::cable_probe_ion_current_density)) {
    auto casted_probe =
        std::any_cast<arb::cable_probe_ion_current_density>(probe);
    std::stringstream locs;
    locs << casted_probe.locations;
    return "cable_probe_ion_current_density";
  }

  if (probe.type() == typeid(arb::cable_probe_total_ion_current_cell)) {
    return "cable_probe_total_ion_current_cell";
  }

  if (probe.type() == typeid(arb::cable_probe_ion_int_concentration)) {
    return "cable_probe_ion_int_concentration";
  }

  if (probe.type() == typeid(arb::cable_probe_ion_int_concentration_cell)) {
    return "cable_probe_ion_int_concentration_cell";
  }

  if (probe.type() == typeid(arb::cable_probe_ion_ext_concentration)) {
    return "cable_probe_ion_ext_concentration";
  }

  if (probe.type() == typeid(arb::cable_probe_ion_ext_concentration_cell)) {
    return "cable_probe_ion_ext_concentration_cell";
  }

  if (probe.type() == typeid(arb::cable_probe_point_state)) {
    return "cable_probe_point_state";
  }

  if (probe.type() == typeid(arb::cable_probe_point_state_cell)) {
    return "cable_probe_point_state_cell";
  }

  return "unknown probe";
}
}  // namespace insite
