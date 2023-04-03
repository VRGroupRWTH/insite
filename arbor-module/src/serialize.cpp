#include <insite/arbor/arbor_spike.h>
#include <insite/arbor/serialize.h>
#include <algorithm>
#include <arbor/cable_cell_param.hpp>
#include <arbor/mechanism.hpp>
#include <arbor/morph/mprovider.hpp>
#include <arbor/morph/place_pwlin.hpp>
#include <arbor/spike_event.hpp>
#include <arbor/util/any_visitor.hpp>
#include <iterator>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

#include <spdlog/spdlog.h>

namespace insite {

void CableBranchesAsJSON(StringBufferWriter& writer,
                         const arb::morphology& morphology) {
  writer.StartArray();
  for (int i = 0; i < morphology.num_branches(); i++) {
    const auto segments = morphology.branch_segments(i);
    const auto num_segments = segments.size();

    writer.StartArray();

    std::stringstream o;
    o << morphology;

    writer.String(o.str().c_str(), o.str().length(), true);
    writer.EndArray();
  }
  writer.EndArray();
}

// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts... {};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct PaintableSerialize {
  PaintableSerialize(std::string type, double value)
      : type(type), value(std::to_string(value)){};

  PaintableSerialize(std::string type, std::string value)
      : type(type), value(value){};
  std::string type;
  std::string value;
};

struct PaintableVisitor {
  PaintableSerialize operator()(const arb::init_membrane_potential& x) {
    return {"init_membrane_potential", x.value};
  };
  PaintableSerialize operator()(const arb::axial_resistivity& x) {
    return {"axial_resistivity", x.value};
  };
  PaintableSerialize operator()(const arb::temperature_K& x) {
    return {"temperature_K", x.value};
  };
  PaintableSerialize operator()(const arb::membrane_capacitance& x) {
    return {"membrane_capacitance", x.value};
  };
  PaintableSerialize operator()(const arb::ion_diffusivity& x) {
    return {"ion_diffusivity", x.value};
  };
  PaintableSerialize operator()(const arb::init_int_concentration& x) {
    return {"init_int_concentration", x.value};
  };
  PaintableSerialize operator()(const arb::init_ext_concentration& x) {
    return {"init_ext_concentration", x.value};
  };
  PaintableSerialize operator()(const arb::init_reversal_potential& x) {
    return {"init_reversal_potential", x.value};
  };
  PaintableSerialize operator()(const arb::density& x) {
    return {"density", x.mech.name()};
  };
  PaintableSerialize operator()(const arb::voltage_process& x) {
    return {"voltage_process", x.mech.name()};
  };
  PaintableSerialize operator()(const arb::scaled_mechanism<arb::density>& x) {
    return {"scaled_mechanism", x.t_mech.mech.name()};
  };
};

struct PlaceableSerialize {
  PlaceableSerialize(){};
  std::string type;
  std::string tag;
  std::string value;
};

struct PlacableVisitor {
  auto operator()(arb::threshold_detector threshold_detector)
      -> PlaceableSerialize {
    PlaceableSerialize serialized;
    serialized.type = "threshold_detector";
    serialized.value = threshold_detector.threshold;
    return serialized;
  };
  auto operator()(arb::synapse synapse) -> PlaceableSerialize {
    PlaceableSerialize serialized;
    serialized.type = "synapse";
    serialized.value = synapse.mech.name();
    return serialized;
  };
  auto operator()(arb::junction junction) -> PlaceableSerialize {
    PlaceableSerialize serialized;
    serialized.type = "junction";
    serialized.value = junction.mech.name();
    return serialized;
  };
  auto operator()(arb::i_clamp clamp) -> PlaceableSerialize {
    PlaceableSerialize serialized;
    serialized.type = "i_clamp";
    serialized.value = clamp.frequency;
    return serialized;
  };
};

void PaintingAsJson(StringBufferWriter& writer,
                    const std::pair<arb::region, arb::paintable>& painting) {
  std::stringstream regionString;
  regionString << painting.first << "\n";

  auto serializedPaintable = std::visit(PaintableVisitor(), painting.second);

  writer.StartObject();
  writer.Key("region");
  writer.String(regionString.str().c_str());

  writer.Key("paintable");
  writer.StartObject();

  writer.Key("type");
  writer.String(serializedPaintable.type.c_str());

  writer.Key("value");
  writer.String(serializedPaintable.value.c_str());

  writer.EndObject();

  writer.EndObject();
}

void PlacementAsJson(
    StringBufferWriter& writer,
    const std::tuple<arb::locset, arb::placeable, arb::cell_tag_type>&
        placement) {
  std::stringstream regionString;
  regionString << std::get<0>(placement);

  auto placeable = std::get<1>(placement);
  auto serializedPlaceable = std::visit(PlacableVisitor(), placeable);

  serializedPlaceable.tag = std::get<2>(placement);

  writer.StartObject();  // Start Placeable Object

  writer.Key("region");
  writer.String(regionString.str().c_str());

  writer.Key("placeable");
  writer.StartObject();

  writer.Key("label");
  writer.String(serializedPlaceable.tag.c_str());

  writer.Key("type");
  writer.String(serializedPlaceable.type.c_str());

  writer.Key("value");
  writer.String(serializedPlaceable.value.c_str());

  writer.EndObject();

  writer.EndObject();  // End Placeable Object
}

void DecoratorAsJSON(StringBufferWriter& writer, const arb::decor& decor) {
  auto paintings = decor.paintings();
  auto placements = decor.placements();

  writer.StartObject();

  spdlog::info("Number of Paintings: {}", paintings.size());
  writer.Key("painting");
  writer.StartArray();
  for (const auto& painting : paintings) {
    PaintingAsJson(writer, painting);
  }
  writer.EndArray();

  spdlog::info("Number of Placements: {}", placements.size());
  writer.Key("placement");
  writer.StartArray();
  for (const auto& placement : placements) {
    PlacementAsJson(writer, placement);
  }
  writer.EndArray();

  writer.EndObject();
}

using reg_map = std::unordered_map<std::string, arb::region>;
using ps_map = std::unordered_map<std::string, arb::locset>;

void RegionAsJson(StringBufferWriter& writer,
                  const reg_map::value_type& region) {
  std::stringstream regionString;
  regionString << region.second;
  writer.StartObject();

  writer.Key("name");
  writer.String(region.first.c_str());

  writer.Key("region");
  writer.String(regionString.str().c_str());

  writer.EndObject();
}

void LocsetsAsJson(StringBufferWriter& writer,
                   const ps_map::value_type& locset) {
  std::stringstream locsetString;
  locsetString << locset.second;

  writer.StartObject();

  writer.Key("name");
  writer.String(locset.first.c_str());

  writer.Key("locset");
  writer.String(locsetString.str().c_str());

  writer.EndObject();
}

void LabelAsJSON(StringBufferWriter& writer,
                 const arb::label_dict& label_dict) {
  writer.StartObject();  // Start Outer Object

  writer.Key("regions");
  writer.StartArray();  // Start [regions, ...] Array
  for (const auto& region : label_dict.regions()) {
    RegionAsJson(writer, region);
  }
  writer.EndArray();  // End Array [regions, ...]

  writer.Key("locset");
  writer.StartArray();  // Start [locsets, ...] Array
  for (const auto& locset : label_dict.locsets()) {
    LocsetsAsJson(writer, locset);
  }
  writer.EndArray();  // End [locsets, ...] Array

  writer.EndObject();  // End Outer Object
}

void CableMorphologyAsJSON(StringBufferWriter& writer,
                           const arb::morphology& morphology) {
  writer.StartObject();
  writer.Key("branches");
  CableBranchesAsJSON(writer, morphology);
  writer.EndObject();
}

std::string CellKindToString(arb::cell_kind cell) {
  switch (cell) {
    case arb::cell_kind::benchmark:
      return "benchmark";
    case arb::cell_kind::cable:
      return "cable";
    case arb::cell_kind::lif:
      return "lif";
    case arb::cell_kind::spike_source:
      return "spike_source";
    default:
      return "unknown type";
  }
}

void LIFCellAsJson(StringBufferWriter& writer, const arb::lif_cell* lif_cell) {
  writer.StartObject();
  writer.Key("tau_m");
  writer.Double(lif_cell->tau_m);

  writer.Key("V_th");
  writer.Double(lif_cell->V_th);

  writer.Key("C_m");
  writer.Double(lif_cell->C_m);

  writer.Key("E_L");
  writer.Double(lif_cell->E_L);

  writer.Key("V_m");
  writer.Double(lif_cell->V_m);

  writer.Key("t_ref");
  writer.Double(lif_cell->t_ref);

  writer.EndObject();
}

void CellDescriptionAsJSON(StringBufferWriter& writer,
                           const arb::util::unique_any* cell_desc) {
  using arb::util::any_cast;

  if (auto lif_cell = arb::util::any_cast<arb::lif_cell>(cell_desc); lif_cell) {
    SPDLOG_INFO("serializing LIF_CELL");
    LIFCellAsJson(writer, lif_cell);

  } else if (auto cable_cell = any_cast<arb::cable_cell>(cell_desc);
             cable_cell) {
    SPDLOG_INFO("serializing CABLE_CELL");
    writer.StartObject();
    writer.Key("morphology");
    CableMorphologyAsJSON(writer, cable_cell->morphology());

    writer.Key("label");
    LabelAsJSON(writer, cable_cell->labels());

    writer.Key("decor");
    DecoratorAsJSON(writer, cable_cell->decorations());

    writer.EndObject();

  } else {
    spdlog::info("UNKNOWN_CELL");
    writer.Null();
  }
}

void SerializeProbe(StringBufferWriter& writer, const ProbeInfoInsite& probe) {
  writer.StartObject();
  writer.Key("cellGid");
  writer.Int(probe.cell);

  writer.Key("cellLid");
  writer.Int(probe.probe_index);

  writer.Key("sourceIndex");
  writer.Int(probe.index);

  writer.Key("probeKind");
  writer.String(probe.kind.c_str());

  writer.Key("hash");
  writer.Int64(probe.hash_);

  writer.Key("globalProbeIndex");
  writer.Int(probe.probe_index_global);

  auto meta_data = HttpBackend::simulation_->get_probe_metadata(
      {probe.cell, probe.probe_index});

  auto* loc =
      arb::util::any_cast<const arb::mlocation*>(meta_data[probe.index].meta);

  auto* cable_list =
      arb::util::any_cast<const arb::mcable_list*>(meta_data[probe.index].meta);

  if (cable_list) {
    writer.Key("locations");
    writer.StartArray();

    for (const auto& cable : *cable_list) {
      writer.StartArray();
      writer.Int(cable.branch);
      writer.Double(cable.prox_pos);
      writer.Double(cable.dist_pos);
      writer.EndArray();
    }

    writer.EndArray();
  }

  if (loc) {
    writer.Key("location");
    writer.StartArray();
    writer.Int(loc->branch);
    writer.Double(loc->pos);
    writer.EndArray();
  }

  writer.EndObject();
}

void SerializeCableCellInfos(StringBufferWriter& writer,
                             arb::util::unique_any* cell_desc) {
  int num_segments = 0;
  auto cable_cell = arb::util::any_cast<arb::cable_cell>(cell_desc);

  for (int j = 0; j < cable_cell->morphology().num_branches(); j++) {
    num_segments += cable_cell->morphology().branch_segments(j).size();
  }

  auto num_branches = cable_cell->morphology().num_branches();

  writer.Key("numBranches");
  writer.Int(num_branches);

  writer.Key("numSegments");
  writer.Int(num_segments);
}

void SerializeCellInfo(StringBufferWriter& writer,
                       int gid,
                       arb::cell_kind& cell_kind,
                       arb::util::unique_any* cell_desc) {
  auto cable_cell = arb::util::any_cast<arb::cable_cell>(cell_desc);

  writer.StartObject();

  writer.Key("gid");
  writer.Int(gid);

  writer.Key("cellKind");
  writer.String(CellKindToString(cell_kind).c_str());

  if (cable_cell) {
    SerializeCableCellInfos(writer, cell_desc);
  }

  writer.EndObject();
}

void SerializeCell(StringBufferWriter& writer,
                   int gid,
                   arb::cell_kind& cell_kind,
                   arb::util::unique_any* cell_desc) {
  writer.StartObject();

  writer.Key("gid");
  writer.Int(gid);

  writer.Key("cellKind");
  writer.String(CellKindToString(cell_kind).c_str());

  writer.Key("cellDescription");
  CellDescriptionAsJSON(writer, cell_desc);

  writer.EndObject();
}

void SerializeSpikes(
    StringBufferWriter& writer,
    const std::vector<arb::basic_spike<arb::cell_member_type>>& spikes,
    SpikeParameter& params) {
  // auto spike_start = spikes.begin();
  std::vector<arb::basic_spike<arb::cell_member_type>>::const_iterator begin;
  std::vector<arb::basic_spike<arb::cell_member_type>>::const_iterator end;
  std::vector<arb::basic_spike<arb::cell_member_type>> spikes_filtered;
  std::copy_if(spikes.begin(), spikes.end(), std::back_inserter(spikes_filtered), [&](auto spike) { return SpikeFulfillsSpikefilter(spike, params); });

  bool reverse_order = false;
  auto skip = params.skip_;
  auto top = params.top_;
  auto number_of_spikes = spikes_filtered.size();
  // auto spike_end = spikes.begin();

  if (!reverse_order || reverse_order == false) {
    begin = spikes_filtered.begin();
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

      end = spikes_filtered.begin();
      std::advance(end, end_offset);
    } else {
      end = spikes_filtered.end();
    }

  } else if (reverse_order == true) {
    end = spikes_filtered.end();
    if (skip) {
      const std::size_t skip_value = skip.value();
      auto start_offset =
          std::clamp(skip_value, static_cast<std::size_t>(0), number_of_spikes);
      std::advance(end, -start_offset);
    }

    if (top) {
      begin = spikes_filtered.end();

      const std::size_t skip_value = skip.value_or(0);
      const std::size_t top_value = top.value();
      auto end_offset =
          std::clamp(skip_value + top_value, static_cast<std::size_t>(0),
                     number_of_spikes);
      std::advance(begin, -end_offset);
    } else {
      begin = spikes_filtered.begin();
    }
  }

  writer.StartObject();
  writer.Key("simulationTimes");
  writer.StartArray();
  for (auto spike = begin; spike != end; ++spike) {
    writer.Double(spike->time);
  }
  writer.EndArray();

  writer.Key("gIds");
  writer.StartArray();
  for (auto spike = begin; spike != end; ++spike) {
    writer.Double(spike->source.gid);
  }
  writer.EndArray();

  writer.EndObject();
}

void SerializeProbeData(rapidjson::Writer<rapidjson::StringBuffer>& writer,
                        const std::vector<insite::ProbeData>& probe_data,
                        ProbeHashType probe_hash,
                        ProbeParameter& params) {
  writer.StartObject();
  writer.Key("gid");
  writer.Int(probe_hash.gid);

  writer.Key("lid");
  writer.Int(probe_hash.lid);

  writer.Key("sourceIndex");
  writer.Int(probe_hash.pid);

  writer.Key("size");
  writer.Int(probe_data.size());

  writer.Key("simulationTimes");
  writer.StartArray();
  for (const auto& sample : probe_data) {
    auto sample_time = sample.time_;
    if (ValueInOptionalRange(sample_time, params.fromTime_, params.toTime_)) {
      writer.Double(sample.time_);
    }
  }
  writer.EndArray();

  writer.Key("gIds");
  writer.StartArray();
  for (const auto& sample : probe_data) {
    auto sample_time = sample.time_;
    if (ValueInOptionalRange(sample_time, params.fromTime_, params.toTime_)) {
      writer.Double(sample.value_);
    }
  }
  writer.EndArray();
  writer.EndObject();
}

}  // namespace insite
