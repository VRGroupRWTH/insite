#include <stdexcept>
#include <string>

// Includes from libnestutil:
#include "compose.hpp"

// Includes from nestkernel:
#include "kernel_manager.h"
#include "simulation_manager.h"
#include "recording_device.h"
#include "vp_manager_impl.h"

// Includes from topology:
#include "topology.h"

// Includes from sli:
#include "dictutils.h"
#include "recording_backend_insite.h"

namespace insite
{

RecordingBackendInsite::RecordingBackendInsite() :
  http_server_("http://0.0.0.0:" + get_port_string(), &data_storage_) {}

RecordingBackendInsite::~RecordingBackendInsite() throw() {}

void RecordingBackendInsite::initialize() {
  std::cout << "RecordingBackendInsite::initialize()\n";
}

void RecordingBackendInsite::finalize() {
  std::cout << "RecordingBackendInsite::finalize()\n";
}

void RecordingBackendInsite::enroll(const nest::RecordingDevice &device,
                                    const DictionaryDatum &params) {
  std::cout << "RecordingBackendInsite::enroll(" << device.get_label() << ")\n";

  if (device.get_type() == nest::RecordingDevice::SPIKE_DETECTOR) {
    data_storage_.CreateSpikeDetectorStorage(device.get_node_id());
  } else if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    // auto id = device.get_node_id();
    // multimeter_infos_.emplace(std::make_pair(id, MultimeterInfo{id, true}));
  }
}

void RecordingBackendInsite::disenroll(const nest::RecordingDevice &device) {
  std::cout << "RecordingBackendInsite::disenroll(" << device.get_label()
            << ")\n";
}

void RecordingBackendInsite::set_value_names(
    const nest::RecordingDevice &device,
    const std::vector<Name> &double_value_names,
    const std::vector<Name> &long_value_names) {
  std::cout << "RecordingBackendInsite::set_value_names()\n";
}

void RecordingBackendInsite::prepare() {
  DictionaryDatum properties(new Dictionary());
  nest::NodeCollectionPTR nodes =
      nest::kernel().node_manager.get_nodes(properties, false);
  data_storage_.SetNodesFromCollection(nodes);

  for (const auto &spike_detector : data_storage_.GetSpikeDetectors()) {
    spike_detector.second->Prepare(nodes);
  }
}

void RecordingBackendInsite::cleanup() {
  std::cout << "RecordingBackendInsite::cleanup()\n";
}

void RecordingBackendInsite::pre_run_hook() {
  data_storage_.SetSimulationTimeRange(
      nest::kernel().simulation_manager.get_simulate_from().get_ms(),
      nest::kernel().simulation_manager.get_simulate_to().get_ms());
}

void RecordingBackendInsite::post_run_hook() {
  std::cout << "RecordingBackendInsite::post_run_hook()\n";
}

void RecordingBackendInsite::post_step_hook() {
  data_storage_.SetCurrentSimulationTime(latest_simulation_time_);
}

void RecordingBackendInsite::write(const nest::RecordingDevice &device,
                                   const nest::Event &event,
                                   const std::vector<double> &double_values,
                                   const std::vector<long> &long_values) {
  const auto sender_gid = event.get_sender_node_id();
  const auto time_stamp = event.get_stamp().get_ms();
  if (device.get_type() == nest::RecordingDevice::SPIKE_DETECTOR) {
    data_storage_.AddSpike(device.get_node_id(), time_stamp, sender_gid);
  }
  // if (device.get_type() == nest::RecordingDevice::MULTIMETER)
  // {
  //   auto device_id = device.get_node_id();
  //   auto &multimeter = multimeter_infos_.at(device_id);
  //   auto &gids = multimeter.gids;

  //   // If the measurement is from a GID we previously do not know, add.
  //   if (!binary_search(gids.begin(), gids.end(), sender_gid))
  //   {
  //     gids.insert(std::lower_bound(gids.begin(), gids.end(), sender_gid),
  //                 sender_gid);
  //     multimeter.needs_update = true;
  //   }

  //   for (std::size_t i = 0; i < double_values.size(); ++i)
  //     data_storage_.AddMultimeterMeasurement(
  //         device_id, multimeter.double_attributes[i], time_stamp, sender_gid,
  //         double_values[i]);
  //   for (std::size_t i = 0; i < long_values.size(); ++i)
  //     data_storage_.AddMultimeterMeasurement(
  //         device_id, multimeter.long_attributes[i], time_stamp, sender_gid,
  //         double(long_values[i]));
  // }
  latest_simulation_time_ = std::max(latest_simulation_time_, time_stamp);

  // NeuronInfo neuron_info;
  // neuron_info.gid = sender_gid;
  // if (!binary_search(neuron_infos_.begin(), neuron_infos_.end(), neuron_info)
  // &&
  //     !binary_search(new_neuron_infos_.begin(), new_neuron_infos_.end(),
  //                    neuron_info))
  // {
  //   neuron_info.gid_collection = event.get_sender().get_nc();

  //   const auto layer = nest::get_layer(neuron_info.gid_collection);
  //   if (layer.get())
  //   {
  //     neuron_info.position = layer->get_position_vector(
  //         neuron_info.gid_collection->find(sender_gid));
  //   }

  //   new_neuron_infos_.insert(
  //       std::lower_bound(new_neuron_infos_.begin(), new_neuron_infos_.end(),
  //                        neuron_info),
  //       neuron_info);
  //   data_storage_.AddNeuronId(neuron_info.gid);
  // }
}

void RecordingBackendInsite::set_status(const DictionaryDatum &params) {
  std::cout << "RecordingBackendInsite::set_status()\n";
}

void RecordingBackendInsite::get_status(DictionaryDatum &params) const {
  std::cout << "RecordingBackendInsite::get_status()\n";
}

void RecordingBackendInsite::check_device_status(
    const DictionaryDatum &params) const {
  std::cout << "RecordingBackendInsite::check_device_status()\n";
}

void RecordingBackendInsite::get_device_defaults(
    DictionaryDatum &params) const {
  std::cout << "RecordingBackendInsite::get_device_defaults()\n";
}

void RecordingBackendInsite::get_device_status(
    const nest::RecordingDevice &device, DictionaryDatum &params) const {
  std::cout << "RecordingBackendInsite::get_device_status()\n";
}

std::string RecordingBackendInsite::get_port_string() const {
  return std::to_string(8000 + nest::kernel().mpi_manager.get_rank());
}

}  // namespace insite
