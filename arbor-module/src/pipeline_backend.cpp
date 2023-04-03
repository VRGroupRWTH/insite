// clang-format off
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/fmt/ranges.h>
// clang-format on

#include <insite/arbor/pipeline_backend.h>
#include <insite/arbor/serialize.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <spdlog/common.h>
#include <algorithm>
#include <any>
#include <arbor/cable_cell.hpp>
#include <arbor/common_types.hpp>
#include <arbor/context.hpp>
#include <arbor/lif_cell.hpp>
#include <arbor/morph/primitives.hpp>
#include <arbor/sampling.hpp>
#include <arbor/schedule.hpp>
#include <arbor/spike.hpp>
#include <arbor/util/any_cast.hpp>
#include <arbor/util/any_ptr.hpp>
#include <arbor/util/any_visitor.hpp>
#include <arbor/util/unique_any.hpp>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <utility>
#include <variant>

#include <insite/arbor/arbor_probe.h>
#include <insite/arbor/arbor_spike.h>
#include <insite/arbor/optionals_util.h>

#include <iostream>
#include <string>

namespace insite {

const arb::recipe* HttpBackend::recipe_ = nullptr;
arb::simulation* HttpBackend::simulation_ = nullptr;

void HttpBackend::AddSimProbesToInsite() {
  try {
    auto num_cells = recipe_->num_cells();
    for (arb::cell_gid_type cell_idx = 0; cell_idx < num_cells; cell_idx++) {
      auto probes = recipe_->get_probes(cell_idx);
      for (arb::cell_lid_type probe_idx = 0; probe_idx < probes.size();
           probe_idx++) {
        auto& probe = probes[probe_idx];

        if (!insite_cell_pred({cell_idx, probe_idx})) {
          continue;
        }

        for (auto meta :
             simulation_->get_probe_metadata({cell_idx, probe_idx})) {
          std::uint64_t hash_ = ProbeHash(cell_idx, probe_idx, meta.index).hash;

          probe_linear_index_to_hash_.push_back(hash_);
          auto probe_linear_index = probe_linear_index_to_hash_.size() - 1;

          SPDLOG_TRACE(
              "Added Probe with hash: {}, gid: {}, lid: {}, index: {}, tag: "
              "{}, meta type name:{} ",
              hash_, meta.id.gid, meta.id.index, meta.index, meta.tag,
              meta.meta.type().name());
          probe_map_.insert(
              std::make_pair(hash_, ProbeInfoInsite(cell_idx, probe_idx, meta.index,
                                                    ProbeKindToString(probe.address), hash_,
                                                    probe_linear_index)));
          data_map_[hash_];
        }
      }
    }

  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
  }
}

HttpBackend::HttpBackend(const arb::context& ctx,
                         const arb::recipe& rcp,
                         arb::simulation& sim,
                         arb::cell_member_predicate pred,
                         arb::schedule sched)
    : context_(ctx), insite_cell_pred(pred), insite_sched(sched) {
  HttpBackend::recipe_ = &rcp;
  HttpBackend::simulation_ = &sim;
  RegisterEndpoints();
  spdlog::cfg::load_env_levels();

  int port = 19000 + arb::rank(context_);

  SPDLOG_INFO("Starting Webserver on port: {}", port);
  app_.stream_threshold(std::numeric_limits<unsigned int>::max());
  crow_server_ = app_.port(port).multithreaded().run_async();
  app_.wait_for_server_start();
  SPDLOG_INFO("Webserver started on port: {}", port);

  auto scalar_sampler = [this](arb::probe_metadata pm, std::size_t n,
                               const arb::sample_record* samples) {
    SPDLOG_TRACE("gid: {}, lid: {}, index: {}, tag: {}, meta type: {}",
                 pm.id.gid, pm.id.index, pm.index, pm.tag,
                 pm.meta.type().name());

    SPDLOG_TRACE("double: {}, sample_range: {}, n: {}",
                 arb::util::any_cast<const double*>(samples[0].data) != nullptr,
                 arb::util::any_cast<const arb::cable_sample_range*>(
                     samples[0].data) != nullptr,
                 n);

    for (std::size_t ix = 0; ix < n; ix++) {
      const double time = samples[ix].time;

      assert((loc != nullptr) || (point_info != nullptr) ||
             (cable_ptr != nullptr) || (point_info_ptr != nullptr));

      const double* double_sample_value =
          arb::util::any_cast<const double*>(samples[ix].data);

      const arb::cable_sample_range* ur =
          arb::util::any_cast<const arb::cable_sample_range*>(samples[ix].data);

      if (double_sample_value) {
        data_map_[ProbeHash(pm.id.gid, pm.id.index, pm.index).hash].push_back(
            {time, *double_sample_value});
      } else {
        const auto& [lo, hi] = *ur;
        auto n_entities = hi - lo;
        for (int i = 0; i < n_entities; i++) {
          data_map_[ProbeHash(pm.id.gid, pm.id.index, pm.index).hash].push_back(
              {time, lo[i]});
        }
      }
    }
    SPDLOG_TRACE("");
  };

  auto vector_sampler = [this](arb::probe_metadata pm, std::size_t n,
                               const arb::sample_record* samples) {
    for (std::size_t ix = 0; ix < n; ix++) {
      auto* loc = arb::util::any_cast<const arb::mlocation*>(pm.meta);
      auto* point_info =
          arb::util::any_cast<const arb::cable_probe_point_info*>(pm.meta);
      auto* cable_ptr = arb::util::any_cast<const arb::mcable_list*>(pm.meta);
      auto* point_info_ptr =
          arb::util::any_cast<const std::vector<arb::cable_probe_point_info>*>(
              pm.meta);
      arb::util::any_cast<const arb::mcable_list*>(pm.meta);
      assert((loc != nullptr) || (point_info != nullptr) ||
             (cable_ptr != nullptr) || (point_info_ptr != nullptr));
      SPDLOG_INFO(
          "double: {}, sample_range: {}",
          arb::util::any_cast<const double*>(samples[ix].data) != nullptr,
          arb::util::any_cast<const arb::cable_sample_range*>(
              samples[ix].data) != nullptr);
    }
  };

  AddSimProbesToInsite();

  sim.add_sampler(insite_cell_pred, insite_sched, scalar_sampler,
                  insite_policy);

  sim.set_epoch_callback([this](double current, double final) {
  });

  sim.set_global_spike_callback([this](auto& spikes) {
    spike_.insert(spike_.end(), spikes.begin(), spikes.end());
  });
}

std::string SerializeCell(arb::lif_cell cell) {
  std::ostringstream celldesc;
  celldesc << "C_m: " << cell.C_m << ", E_L: " << cell.E_L << std::endl;
  return celldesc.str();
}

crow::response HttpBackend::GetCellInfos(const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartObject();
  writer.Key("CellInfos");
  writer.StartArray();

  for (int gid = 0; gid < recipe_->num_cells(); gid++) {
    auto cell_desc = recipe_->get_cell_description(gid);
    auto cell_kind = recipe_->get_cell_kind(gid);

    SerializeCellInfo(writer, gid, cell_kind, &cell_desc);
  }
  writer.EndArray();
  writer.EndObject();

  return crow::response{buffer.GetString()};
}

crow::response HttpBackend::GetCells(const crow::request& request) {
  SPDLOG_INFO("GetCells callback, serializing {} cells: {}",
              recipe_->num_cells());

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartObject();
  writer.Key("cells");
  writer.StartArray();

  for (int gid = 0; gid < recipe_->num_cells(); gid++) {
    auto cell_desc = recipe_->get_cell_description(gid);
    auto cell_kind = recipe_->get_cell_kind(gid);

    SerializeCell(writer, gid, cell_kind, &cell_desc);
  }
  writer.EndArray();
  writer.EndObject();

  return crow::response{buffer.GetString()};
}

tl::optional<std::uint64_t> HttpBackend::UIdToHash(int uid) {
  if (uid < probe_linear_index_to_hash_.size()) {
    auto hash = probe_linear_index_to_hash_[uid];
    SPDLOG_TRACE("UIDs: {}, converted to hashes: {} ", uid, hash);
    return hash;
  }
  return tl::nullopt;
}

std::vector<std::uint64_t> HttpBackend::UIdToHash(std::vector<int>& uids) {
  std::vector<std::uint64_t> hashes;
  for (const auto& uid : uids) {
    if (uid < probe_linear_index_to_hash_.size()) {
      auto hash = probe_linear_index_to_hash_[uid];
      hashes.push_back(hash);
      SPDLOG_TRACE("UIDs: {}, converted to hashes: {} ", uid, hash);
    }
  }
  return hashes;
}

crow::response HttpBackend::GetProbes(const crow::request& request) {
  ProbeParameter params(request.url_params);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartObject();
  writer.Key("probes");
  writer.StartArray();
  for (const auto& probe : probe_map_) {
    ProbeHashType probe_hash = {probe.first};

    if (params.hash_.empty() && !params.u_id_.empty()) {
      params.hash_ = UIdToHash(params.u_id_);
    }

    if (!ProbeFulfillsProbefilter(probe_hash, params)) {
      continue;
    }

    SerializeProbe(writer, probe.second);
  }
  writer.EndArray();
  writer.EndObject();

  return crow::response{buffer.GetString()};
}

crow::response HttpBackend::GetProbeData(const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  ProbeParameter params(request.url_params);

  writer.StartObject();
  writer.Key("probeData");
  writer.StartArray();
  for (const auto& probe : data_map_) {
    ProbeHashType probe_hash = {probe.first};

    if (params.hash_.empty() && !params.u_id_.empty()) {
      params.hash_ = UIdToHash(params.u_id_);
    }

    if (!ProbeFulfillsProbefilter(probe_hash, params)) {
      continue;
    }

    SerializeProbeData(writer, probe.second, probe_hash, params);
  }

  writer.EndArray();
  writer.EndObject();
  assert(writer.IsComplete());

  return {buffer.GetString()};
}

crow::response HttpBackend::GetSpikes(const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  SpikeParameter params(request.url_params);

  SerializeSpikes(writer, spike_, params);

  return {buffer.GetString()};
}

void HttpBackend::RegisterEndpoints() {
  CROW_ROUTE_LAMBA(app_, "/probes/", GetProbes);
  CROW_ROUTE_LAMBA(app_, "/probe_data/", GetProbeData);
  CROW_ROUTE_LAMBA(app_, "/spikes/", GetSpikes);
  CROW_ROUTE_LAMBA(app_, "/cells/", GetCells);
  CROW_ROUTE_LAMBA(app_, "/cell_info/", GetCellInfos);
  CROW_ROUTE_LAMBA(app_, "/version", [](const crow::request& req) {
    return crow::response("1.0.5");
  });
}

}  // namespace insite
