#include <stdexcept>
#include <string>

// Includes from libnestutil:
#include "compose.hpp"

// Includes from nestkernel:
#include "kernel_manager.h"
#include "recording_device.h"
#include "simulation_manager.h"
#include "vp_manager_impl.h"

// Includes from topology:
#include "topology.h"

// Includes from sli:
#include "dictutils.h"
#include "recording_backend_insite.h"

namespace insite {

RecordingBackendInsite::RecordingBackendInsite()
    : http_server_("http://0.0.0.0:" + get_port_string(), &data_storage_) {}

RecordingBackendInsite::~RecordingBackendInsite() throw() {}

void RecordingBackendInsite::initialize() {
}

void RecordingBackendInsite::finalize() {
}

void RecordingBackendInsite::enroll(const nest::RecordingDevice &device,
                                    const DictionaryDatum &params) {
  if (device.get_type() == nest::RecordingDevice::SPIKE_DETECTOR) {
    std::cout << "[insite] enroll spike detector (" << device.get_label() << ") with id " << device.get_node_id() << "\n";
    data_storage_.CreateSpikeDetectorStorage(device.get_node_id());
  } else if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    std::cout << "[insite] enroll multimeter (" << device.get_label() << ") with id " << device.get_node_id() << "\n";
    data_storage_.CreateMultimeterStorage(device.get_node_id());
  }
}

void RecordingBackendInsite::disenroll(const nest::RecordingDevice &device) {
}

void RecordingBackendInsite::set_value_names(
    const nest::RecordingDevice &device,
    const std::vector<Name> &double_value_names,
    const std::vector<Name> &long_value_names) {
  if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    data_storage_.GetMultimeterStorage(device.get_node_id())->SetAttributes(double_value_names, long_value_names);
  }
}

void RecordingBackendInsite::prepare() {
  DictionaryDatum properties(new Dictionary());
  // Set the last parameter to false below to only get locally simulated nodes
  nest::NodeCollectionPTR nodes = nest::kernel().node_manager.get_nodes(properties, true);
  data_storage_.SetNodesFromCollection(nodes);

  for (const auto &spike_detector : data_storage_.GetSpikeDetectors()) {
    spike_detector.second->Prepare(nodes);
  }
  for (const auto &multimeter : data_storage_.GetMultimeters()) {
    multimeter.second->Prepare(nodes);
  }
}

void RecordingBackendInsite::cleanup() {
}

void RecordingBackendInsite::pre_run_hook() {
  data_storage_.SetSimulationTimeRange(
      nest::kernel().simulation_manager.get_simulate_from().get_ms(),
      nest::kernel().simulation_manager.get_simulate_to().get_ms());
}

void RecordingBackendInsite::post_run_hook() {
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
  } else if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    data_storage_.AddMultimeterMeasurement(device.get_node_id(), time_stamp, sender_gid, double_values, long_values);
  }
  latest_simulation_time_ = std::max(latest_simulation_time_, time_stamp);
}

void RecordingBackendInsite::set_status(const DictionaryDatum &params) {
}

void RecordingBackendInsite::get_status(DictionaryDatum &params) const {
}

void RecordingBackendInsite::check_device_status(const DictionaryDatum &params) const {
}

void RecordingBackendInsite::get_device_defaults(DictionaryDatum &params) const {
}

void RecordingBackendInsite::get_device_status(const nest::RecordingDevice &device, DictionaryDatum &params) const {
}

std::string RecordingBackendInsite::get_port_string() const {
  // TODO: Make this configurable
  return std::to_string(8000 + nest::kernel().mpi_manager.get_rank());
}

}  // namespace insite
