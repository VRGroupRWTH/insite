/*
 * A miniapp that demonstrates how to make a ring model
 *
 */

#include <any>
#include <arbor/sampling.hpp>
#include <arbor/schedule.hpp>
#include <cassert>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <nlohmann/json.hpp>

#include <arborio/label_parse.hpp>

#include <arbor/assert_macro.hpp>
#include <arbor/cable_cell.hpp>
#include <arbor/common_types.hpp>
#include <arbor/context.hpp>
#include <arbor/load_balance.hpp>
#include <arbor/profile/meter_manager.hpp>
#include <arbor/profile/profiler.hpp>
#include <arbor/recipe.hpp>
#include <arbor/simple_sampler.hpp>
#include <arbor/simulation.hpp>
#include <arbor/version.hpp>

#include <arborenv/default_env.hpp>
#include <arborenv/gpu_env.hpp>

#include <sup/ioutil.hpp>
#include <sup/json_meter.hpp>
#include <sup/json_params.hpp>

#include "branch_cell.hpp"

#include <insite/arbor/pipeline_backend.h>

#ifdef ARB_MPI_ENABLED
#include <mpi.h>
#include <arborenv/with_mpi.hpp>
#endif

struct ring_params {
  ring_params() = default;

  std::string name = "default";
  unsigned num_cells = 3;
  double min_delay = 10;
  double duration = 1000;
  cell_parameters cell;
};

ring_params read_options(int argc, char** argv);
using arb::cell_gid_type;
using arb::cell_kind;
using arb::cell_lid_type;
using arb::cell_member_type;
using arb::cell_size_type;
using arb::time_type;

// Generate a cell.
arb::cable_cell branch_cell(arb::cell_gid_type gid,
                            const cell_parameters& params);

class ring_recipe : public arb::recipe {
 public:
  ring_recipe(unsigned num_cells, cell_parameters params, unsigned min_delay)
      : num_cells_(num_cells), cell_params_(params), min_delay_(min_delay) {
    gprop_.default_parameters = arb::neuron_parameter_defaults;
  }

  cell_size_type num_cells() const override { return num_cells_; }

  arb::util::unique_any get_cell_description(cell_gid_type gid) const override {
    return branch_cell(gid, cell_params_);
  }

  cell_kind get_cell_kind(cell_gid_type gid) const override {
    return cell_kind::cable;
  }

  // Each cell has one incoming connection, from cell with gid-1.
  std::vector<arb::cell_connection> connections_on(
      cell_gid_type gid) const override {
    std::vector<arb::cell_connection> cons;
    cell_gid_type src = gid ? gid - 1 : num_cells_ - 1;
    cons.push_back(arb::cell_connection({src, "detector"}, {"primary_syn"},
                                        event_weight_, min_delay_));
    return cons;
  }

  // Return one event generator on gid 0. This generates a single event that
  // will kick start the spiking.
  std::vector<arb::event_generator> event_generators(
      cell_gid_type gid) const override {
    std::vector<arb::event_generator> gens;
    if (!gid) {
      gens.push_back(arb::explicit_generator({"primary_syn"}, event_weight_,
                                             std::vector<float>{1.0f}));
    }
    return gens;
  }

  std::vector<arb::probe_info> get_probes(cell_gid_type gid) const override {
    // Measure membrane voltage at end of soma.
    arb::mlocation loc{0, 0.5};
    arb::locset lset{{{0, 0.3}, {0, 0.4}, {0, 0.6}}};

    return {arb::cable_probe_membrane_voltage{lset},
            arb::cable_probe_axial_current{loc}};
  }

  std::any get_global_properties(arb::cell_kind) const override {
    return gprop_;
  }

 private:
  cell_size_type num_cells_;
  cell_parameters cell_params_;
  double min_delay_;
  float event_weight_ = 0.05;
  arb::cable_cell_global_properties gprop_;
};

int main(int argc, char** argv) {
  try {
    bool root = true;

    arb::proc_allocation resources;
    resources.num_threads = arbenv::default_concurrency();

#ifdef ARB_MPI_ENABLED
    arbenv::with_mpi guard(argc, argv, false);
    // resources.gpu_id = arbenv::find_private_gpu(MPI_COMM_WORLD);
    auto context = arb::make_context(resources, MPI_COMM_WORLD);
    root = arb::rank(context) == 0;
#else
    resources.gpu_id = arbenv::default_gpu();
    auto context = arb::make_context(resources);
#endif

#ifdef ARB_PROFILE_ENABLED
    arb::profile::profiler_initialize(context);
#endif

    std::cout << sup::mask_stream(root);

    // Print a banner with information about hardware configuration
    std::cout << "gpu:      " << (has_gpu(context) ? "yes" : "no") << "\n";
    std::cout << "threads:  " << num_threads(context) << "\n";
    std::cout << "mpi:      " << (has_mpi(context) ? "yes" : "no") << "\n";
    std::cout << "ranks:    " << num_ranks(context) << "\n"
              << std::endl;

    auto params = read_options(argc, argv);

    arb::profile::meter_manager meters;
    meters.start(context);

    // Create an instance of our recipe.
    ring_recipe recipe(params.num_cells, params.cell, params.min_delay);

    // Construct the model.
    auto decomposition = arb::partition_load_balance(recipe, context);
    arb::simulation sim(recipe, context, decomposition);

    insite::HttpBackend insite_backend(context, recipe, sim, arb::all_probes,
                                       arb::regular_schedule(1.0));

    std::cout << "running simulation\n"
              << std::endl;

    sim.run(params.duration, 0.025);

    std::string line1;
    std::getline(std::cin, line1);
    while (1) {
      sleep(10000);
    }
  } catch (std::exception& e) {
    std::cerr << "exception caught in ring miniapp: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

ring_params read_options(int argc, char** argv) {
  using sup::param_from_json;

  ring_params params;
  if (argc < 2) {
    std::cout << "Using default parameters.\n";
    return params;
  }
  if (argc > 2) {
    throw std::runtime_error(
        "More than one command line option is not permitted.");
  }

  std::string fname = argv[1];
  std::cout << "Loading parameters from file: " << fname << "\n";
  std::ifstream f(fname);

  if (!f.good()) {
    throw std::runtime_error("Unable to open input parameter file: " + fname);
  }

  nlohmann::json json;
  f >> json;

  param_from_json(params.name, "name", json);
  param_from_json(params.num_cells, "num-cells", json);
  param_from_json(params.duration, "duration", json);
  param_from_json(params.min_delay, "min-delay", json);
  params.cell = parse_cell_parameters(json);

  if (!json.empty()) {
    for (auto it = json.begin(); it != json.end(); ++it) {
      std::cout << "  Warning: unused input parameter: \"" << it.key()
                << "\"\n";
    }
    std::cout << "\n";
  }

  return params;
}
