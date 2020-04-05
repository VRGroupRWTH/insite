#include <stdexcept>
#include <string>

// Includes from libnestutil:
#include "compose.hpp"

// Includes from nestkernel:
#include "kernel_manager.h"
#include "recording_device.h"
#include "vp_manager_impl.h"

// Includes from topology:
#include "topology.h"

// Includes from sli:
#include "dictutils.h"
#include "recording_backend_insite.h"

namespace insite {

namespace {

std::string ReadDatabaseHost() {
  std::ifstream host_file("database_host.txt");
  if (host_file.is_open()) {
    std::string database_host;
    std::getline(host_file, database_host);
    return database_host;
  } else {
    return "database";
  }
}

}  // namespace

RecordingBackendInsite::RecordingBackendInsite()
    : data_storage_("tgest"),
      database_connection_("postgresql://postgres@" + ReadDatabaseHost()),
      http_server_("http://0.0.0.0:" + get_port_string(), &data_storage_, "postgresql://postgres@" + ReadDatabaseHost()) {
  pqxx::work txn(database_connection_);
  simulation_node_id_ = txn.exec1(
                               "INSERT INTO nest_simulation_node (address) "
                               "VALUES ('http://insite-nest-module:" +
                               get_port_string() +
                               "') "
                               "RETURNING id;")[0]
                            .as<int>();
  std::cout << "Simulation node registered to database. Node ID: "
            << simulation_node_id_ << std::endl;
  txn.commit();
}

RecordingBackendInsite::~RecordingBackendInsite() throw() {}

void RecordingBackendInsite::initialize() {
  std::cout << "RecordingBackendInsite::initialize()\n";
}

void RecordingBackendInsite::finalize() {
  std::cout << "RecordingBackendInsite::finalize()\n";
}

void RecordingBackendInsite::enroll(const nest::RecordingDevice& device,
                                    const DictionaryDatum& params) {
  std::cout << "RecordingBackendInsite::enroll(" << device.get_label() << ")\n";

  if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    auto id = device.get_node_id();
    multimeter_infos_.emplace(std::make_pair(id, MultimeterInfo{id, true}));
  }
}

void RecordingBackendInsite::disenroll(const nest::RecordingDevice& device) {
  std::cout << "RecordingBackendInsite::disenroll(" << device.get_label()
            << ")\n";
}

void RecordingBackendInsite::set_value_names(
    const nest::RecordingDevice& device,
    const std::vector<Name>& double_value_names,
    const std::vector<Name>& long_value_names) {
  std::cout << "RecordingBackendInsite::set_value_names()\n";

  if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    auto& multimeter = multimeter_infos_.at(device.get_node_id());

    std::stringstream multimeter_query;
    multimeter_query << "INSERT INTO nest_multimeter (id, attributes) "
                     << "VALUES (" << device.get_node_id() << ",\'{";

    bool first = true;
    for (auto& name : double_value_names) {
      const auto& name_string = name.toString();
      multimeter.double_attributes.push_back(name_string);

      multimeter_query << (first ? "" : ",") << '\"' << name_string << "\"";
      first = false;
    }
    for (auto& name : long_value_names) {
      const auto& name_string = name.toString();
      multimeter.long_attributes.push_back(name_string);

      multimeter_query << (first ? "" : ",") << '\"' << name_string << "\"";
      first = false;
    }
    multimeter_query << "}\') ON CONFLICT DO NOTHING;";

    multimeter.needs_update = true;

    pqxx::work txn(database_connection_);
    txn.exec0(multimeter_query.str());
    txn.commit();
  }
}

void RecordingBackendInsite::prepare() {}

void RecordingBackendInsite::cleanup() {
  std::cout << "RecordingBackendInsite::cleanup()\n";
}

void RecordingBackendInsite::pre_run_hook() {
  std::cout << "RecordingBackendInsite::pre_run_hook()\n";
}

void RecordingBackendInsite::post_run_hook() {
  std::cout << "RecordingBackendInsite::post_run_hook()\n";
}

void RecordingBackendInsite::post_step_hook() {
  // // Send simulation time
  // {
  //   pqxx::work txn(database_connection_);
  //   txn.exec0(
  //       "UPDATE nest_simulation_node "
  //       "SET current_simulation_time = " +
  //       std::to_string(latest_simulation_time_) +
  //       ""
  //       "WHERE id = " +
  //       std::to_string(simulation_node_id_));
  //   txn.commit();
  // }
  data_storage_.SetCurrentSimulationTime(latest_simulation_time_);

  if (new_neuron_infos_.size() > 0) {
    std::stringstream neuron_query;
    neuron_query << "INSERT INTO nest_neuron (id, simulation_node_id, "
                    "population_id, position) "
                 << "VALUES ";
    for (auto& neuron_info : new_neuron_infos_) {
      const bool first = neuron_info.gid == new_neuron_infos_[0].gid;
      if (!first) {
        neuron_query << ",";
      }

      uint64_t population_id = 0;
      for (const nest::NodeIDTriple& node_id_triple :
           *neuron_info.gid_collection.get()) {
        population_id ^= node_id_triple.node_id * 938831;
      }

      neuron_query << "(" << neuron_info.gid << "," << simulation_node_id_
                   << "," << population_id % 0x800000;

      const auto position_size = neuron_info.position.size();
      if (position_size > 0) {
        assert(position_size <= 3);
        neuron_query << ",\'{";
        for (size_t i = 0; i < position_size; ++i) {
          if (i > 0) {
            neuron_query << ",";
          }
          neuron_query << neuron_info.position[i];
        }
        neuron_query << "}\'";
      } else {
        neuron_query << ",NULL";
      }
      neuron_query << ")";
    }
    neuron_query << ";";

    pqxx::work txn(database_connection_);
    txn.exec0(neuron_query.str());
    txn.commit();

    neuron_infos_.insert(neuron_infos_.end(), new_neuron_infos_.begin(),
                         new_neuron_infos_.end());
    std::sort(neuron_infos_.begin(), neuron_infos_.end());
    new_neuron_infos_.clear();
  }

  // Send multimeter info
  // for (auto& kvp : multimeter_infos_) {
  //   auto& multimeter = kvp.second;
  //   if (!multimeter.needs_update) continue;
  //   multimeter.needs_update = false;

  //   if (multimeter.gids.size() > 0) {
  //     std::stringstream neuron_multimeter_query;
  //     neuron_multimeter_query
  //         << "INSERT INTO nest_neuron_multimeter (neuron_id, multimeter_id) "
  //         << "VALUES ";

  //     for (const auto& neuron_id : multimeter.gids) {
  //       const bool first = neuron_id == multimeter.gids[0];
  //       neuron_multimeter_query << (first ? "" : ",") << "(" << neuron_id << ","
  //                               << multimeter.device_id << ")";
  //     }

  //     neuron_multimeter_query << " ON CONFLICT DO NOTHING;";

  //     pqxx::work txn(database_connection_);
  //     txn.exec0(neuron_multimeter_query.str());
  //     txn.commit();
  //   }
  // }
}

void RecordingBackendInsite::write(const nest::RecordingDevice& device,
                                   const nest::Event& event,
                                   const std::vector<double>& double_values,
                                   const std::vector<long>& long_values) {
  const auto sender_gid = event.get_sender_node_id();
  const auto time_stamp = event.get_stamp().get_ms();
  if (device.get_type() == nest::RecordingDevice::SPIKE_DETECTOR) {
    data_storage_.AddSpike(time_stamp, sender_gid);
  }
  if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    auto device_id = device.get_node_id();
    auto& multimeter = multimeter_infos_.at(device_id);
    auto& gids = multimeter.gids;

    // If the measurement is from a GID we previously do not know, add.
    if (!binary_search(gids.begin(), gids.end(), sender_gid)) {
      gids.insert(std::lower_bound(gids.begin(), gids.end(), sender_gid),
                  sender_gid);
      multimeter.needs_update = true;
    }

    for (std::size_t i = 0; i < double_values.size(); ++i)
      data_storage_.AddMultimeterMeasurement(
          device_id, multimeter.double_attributes[i], time_stamp, sender_gid,
          double_values[i]);
    for (std::size_t i = 0; i < long_values.size(); ++i)
      data_storage_.AddMultimeterMeasurement(
          device_id, multimeter.long_attributes[i], time_stamp, sender_gid,
          double(long_values[i]));
  }
  latest_simulation_time_ = std::max(latest_simulation_time_, time_stamp);

  NeuronInfo neuron_info;
  neuron_info.gid = sender_gid;
  if (!binary_search(neuron_infos_.begin(), neuron_infos_.end(), neuron_info) &&
      !binary_search(new_neuron_infos_.begin(), new_neuron_infos_.end(),
                     neuron_info)) {
    neuron_info.gid_collection = event.get_sender().get_nc();

    const auto layer = nest::get_layer(neuron_info.gid_collection);
    if (layer.get()) {
      neuron_info.position = layer->get_position_vector(
          neuron_info.gid_collection->find(sender_gid));
    }

    new_neuron_infos_.insert(
        std::lower_bound(new_neuron_infos_.begin(), new_neuron_infos_.end(),
                         neuron_info),
        neuron_info);
    data_storage_.AddNeuronId(neuron_info.gid);
  }
}

void RecordingBackendInsite::set_status(const DictionaryDatum& params) {
  std::cout << "RecordingBackendInsite::set_status()\n";
}

void RecordingBackendInsite::get_status(DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::get_status()\n";
}

void RecordingBackendInsite::check_device_status(
    const DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::check_device_status()\n";
}

void RecordingBackendInsite::get_device_defaults(
    DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::get_device_defaults()\n";
}

void RecordingBackendInsite::get_device_status(
    const nest::RecordingDevice& device, DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::get_device_status()\n";
}

std::string RecordingBackendInsite::get_port_string() const {
  return std::to_string(8000 + nest::kernel().mpi_manager.get_rank());
}
}  // namespace insite
