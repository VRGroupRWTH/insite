#include <stdexcept>

// Includes from libnestutil:
#include "compose.hpp"

// Includes from nestkernel:
#include "kernel_manager.h"
#include "recording_device.h"
#include "vp_manager_impl.h"

// Includes from sli:
#include "dictutils.h"
#include "recording_backend_insite.h"

namespace insite {

RecordingBackendInsite::RecordingBackendInsite()
    : data_storage_("tgest"),
      http_server_("http://0.0.0.0:" + get_port_string(), &data_storage_),
      info_node_("http://info-node:8080"),
      address_("insite-nest-module:" + get_port_string()) {
  web::uri_builder builder("/node");
  builder.append_query("node_type", "nest_simulation", true);
  builder.append_query("address", address_, true);

  try {
    info_node_.request(web::http::methods::PUT, builder.to_string())
        .then([](const web::http::http_response& response) {
          if (response.status_code() != web::http::status_codes::OK) {
            throw std::runtime_error(response.to_string());
          }
        })
        .wait();
  } catch (const std::exception& exception) {
    std::cerr << "Failed to register to info node: \n"
              << exception.what() << "\n"
              << std::endl;
    throw;
  }
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
  // Send simulation time
  {
    web::uri_builder builder("/current_time");
    builder.append_query("time", latest_simulation_time_, false);
    builder.append_query("node_address", address_, true);

    info_node_.request(web::http::methods::PUT, builder.to_string())
        .then([](const web::http::http_response& response) {
          if (response.status_code() != web::http::status_codes::OK) {
            std::cerr << "Failed to send time to info node: \n"
                      << response.to_string() << "\n"
                      << std::endl;
            throw std::runtime_error(response.to_string());
          }
        });
  }

  // Send new gids
  if (new_gids_.size() > 0) {
    web::uri_builder builder("/gids");
    for (auto gid : new_gids_) {
      builder.append_query("gids", gid, false);
    }
    builder.append_query("address", address_, true);

    info_node_.request(web::http::methods::PUT, builder.to_string())
        .then([](const web::http::http_response& response) {
          if (response.status_code() != web::http::status_codes::OK) {
            std::cerr << "Failed to send gids to info node: \n"
                      << response.to_string() << "\n"
                      << std::endl;
            throw std::runtime_error(response.to_string());
          }
        });

    gids_.insert(gids_.end(), new_gids_.begin(), new_gids_.end());
    std::sort(gids_.begin(), gids_.end());
    new_gids_.clear();
  }

  // Send new collections
  for (const auto& node_collection : node_collections_to_register_) {
    web::uri_builder builder("/population");
    for (auto node_id_triple : *node_collection) {
      builder.append_query("gids", node_id_triple.node_id, false);
    }

    info_node_.request(web::http::methods::PUT, builder.to_string())
        .then([this,
               node_collection](const web::http::http_response& response) {
          if (response.status_code() != web::http::status_codes::OK) {
            std::cerr << "Failed to send population to info node: \n"
                      << response.to_string() << "\n"
                      << std::endl;
            throw std::runtime_error(response.to_string());
          } else {
            response.extract_json().then(
                [this, node_collection](const web::json::value& population_id) {
                  registered_node_collections_[node_collection] = population_id.as_number().to_int64();
                });
          }
        });
  }
}

void RecordingBackendInsite::write(const nest::RecordingDevice& device,
                                   const nest::Event& event,
                                   const std::vector<double>& double_values,
                                   const std::vector<long>& long_values) {
  const auto sender_gid_ = event.get_sender_node_id();
  const auto time_stamp = event.get_stamp().get_steps();
  if (device.get_type() == nest::RecordingDevice::SPIKE_DETECTOR) {
    data_storage_.AddSpike(time_stamp, sender_gid_);
  }
  latest_simulation_time_ = std::max(latest_simulation_time_, time_stamp);
  if (!binary_search(gids_.begin(), gids_.end(), sender_gid_) &&
      !binary_search(new_gids_.begin(), new_gids_.end(), sender_gid_)) {
    new_gids_.insert(
        std::lower_bound(new_gids_.begin(), new_gids_.end(), sender_gid_),
        sender_gid_);

    // Check if the node collection (population) is already sent to the
    // info-node
    const auto sender_node_collection = event.get_sender().get_nc();
    if (registered_node_collections_.count(sender_node_collection) == 0 &&
        !binary_search(node_collections_to_register_.begin(),
                       node_collections_to_register_.end(),
                       sender_node_collection)) {
        node_collections_to_register_.insert(
            std::lower_bound(node_collections_to_register_.begin(),
                             node_collections_to_register_.end(),
                             sender_node_collection),
            sender_node_collection);
      }
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
