#pragma once

#include <arbor/sampling.hpp>
#include <arbor/schedule.hpp>
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <crow.h>
#include <insite/arbor/arbor_probe.h>
#include <insite/arbor/parameters.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <arbor/common_types.hpp>
#include <arbor/context.hpp>
#include <arbor/recipe.hpp>
#include <arbor/simulation.hpp>
#include <thread>

#ifdef ARB_MPI_ENABLED
#include <mpi.h>
#include <arborenv/with_mpi.hpp>
#endif

#define CROW_ROUTE_LAMBA(APP, ENDPOINT, FUNCTION) \
  CROW_ROUTE(APP, ENDPOINT)                       \
  ([this](const crow::request& request) { return FUNCTION(request); });

namespace insite {

typedef rapidjson::Writer<rapidjson::StringBuffer> StringBufferWriter;

struct ProbeInfoInsite {
  ProbeInfoInsite(ProbeInfoInsite const& probe) = default;
  ProbeInfoInsite(ProbeInfoInsite&& probe) = default;
  ProbeInfoInsite(arb::cell_gid_type cell,
                  arb::cell_lid_type probe_index,
                  unsigned int index,
                  std::string kind,
                  std::uint64_t hash_,
                  int probe_index_global)
      : probe_index_global(probe_index_global),
        hash_(hash_),
        cell(cell),
        probe_index(probe_index),
        index(index),
        kind(kind){};
  arb::cell_gid_type cell;
  arb::cell_lid_type probe_index;
  unsigned int index;
  std::string kind;
  int probe_index_global;
  std::uint64_t hash_;
};

struct ProbeData {
  double time_;
  double value_;
};

class HttpBackend {
 public:
  HttpBackend(const arb::context& ctx,
              const arb::recipe& rcp,
              arb::simulation& sim,
              arb::cell_member_predicate cell_pred,
              arb::schedule sched);
  ~HttpBackend() {
  }
  void RegisterEndpoints();

  static arb::simulation* simulation_;
  static const arb::recipe* recipe_;

  arb::cell_member_predicate insite_cell_pred = arb::all_probes;
  arb::schedule insite_sched;
  arb::sampling_policy insite_policy = arb::sampling_policy::lax;

 private:
  std::vector<arb::spike> spike_;

  // Arbor HTTP Endpoints
  crow::response GetSpikes(const crow::request& request);
  crow::response GetProbes(const crow::request& request);
  crow::response GetCells(const crow::request& request);
  crow::response GetCellInfos(const crow::request& request);
  crow::response GetProbeData(const crow::request& request);

  crow::SimpleApp app_;
  std::future<void> crow_server_;

  tl::optional<std::uint64_t> UIdToHash(int uid);
  std::vector<std::uint64_t> UIdToHash(std::vector<int>& uid);

  void AddSimProbesToInsite();

  arb::time_type last_timstep_;
  const arb::context& context_;
  std::thread websocket_thread_;
  std::vector<std::uint64_t> probe_linear_index_to_hash_{};
  std::unordered_map<std::uint64_t, ProbeInfoInsite> probe_map_{};
  std::unordered_map<std::uint64_t, std::vector<ProbeData>> data_map_{};
};
}  // namespace insite
