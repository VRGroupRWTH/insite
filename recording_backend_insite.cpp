#include <stdexcept>

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
    
    for (auto& name : double_value_names)
      multimeter.double_attributes.push_back(name.toString());
    for (auto& name : long_value_names)
      multimeter.long_attributes.push_back(name.toString());

    multimeter.needs_update = true;    
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
        }).wait(); // TODO: this wait definitely needs to go!
  }

  if (new_neuron_infos_.size() > 0) {
    // Send new gids
    web::uri_builder builder("/gids");
    for (auto& neuron_info : new_neuron_infos_) {
      builder.append_query("gids", neuron_info.gid, false);
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
        }).wait();

    // Send new properties
    builder = web::uri_builder("/neuron_properties");
    web::json::value request_body =
        web::json::value::array(new_neuron_infos_.size());
    for (std::size_t i = 0; i < new_neuron_infos_.size(); ++i) {
      auto& neuron_info = new_neuron_infos_[i];
      request_body[i]["gid"] = neuron_info.gid;
      request_body[i]["properties"] = web::json::value();
      if (!neuron_info.position.empty())
        request_body[i]["properties"]["position"] =
            web::json::value::array(std::vector<web::json::value>(
                neuron_info.position.begin(), neuron_info.position.end()));
    }

    info_node_
        .request(web::http::methods::PUT, builder.to_string(), request_body)
        .then([](const web::http::http_response& response) {
          if (response.status_code() != web::http::status_codes::OK) {
            std::cerr << "Failed to send neuron properties to info node: \n"
                      << response.to_string() << "\n"
                      << std::endl;
            throw std::runtime_error(response.to_string());
          }
        }).wait();

    neuron_infos_.insert(neuron_infos_.end(), new_neuron_infos_.begin(),
                         new_neuron_infos_.end());
    std::sort(neuron_infos_.begin(), neuron_infos_.end());
    new_neuron_infos_.clear();
  }

  // Send multimeter info
  for (auto& kvp : multimeter_infos_) {
    auto& multimeter = kvp.second;
    if (!multimeter.needs_update)
      continue;
    multimeter.needs_update = false;
    
    web::uri_builder builder("/multimeter_info");
    builder.append_query("id", multimeter.device_id, true);
    for (auto attribute : multimeter.double_attributes)
      builder.append_query("attributes", attribute, true);
    for (auto attribute : multimeter.long_attributes)
      builder.append_query("attributes", attribute, true);
    for (auto gid : multimeter.gids)
      builder.append_query("gids", gid, false);
    
    try {
      info_node_.request(web::http::methods::PUT, builder.to_string())
          .then([](const web::http::http_response& response) {
            if (response.status_code() != web::http::status_codes::OK) {
              throw std::runtime_error(response.to_string());
            }
          })
          .wait();
    } catch (const std::exception& exception) {
      std::cerr << "Failed to put multimeter info: \n"
                << exception.what() << "\n"
                << std::endl;
      throw;
    }
  }
  // Send new collections
  for (const auto& node_collection : node_collections_to_register_) {
    web::uri_builder builder("/populations");
    for (auto node_id_triple : *node_collection) {
      builder.append_query("gids", node_id_triple.node_id, false);
    }

    // Initialize to "invalid" state
    registered_node_collections_[node_collection] = -1;
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
                  std::cout << "Got id for node collection " << node_collection
                            << ": " << population_id << std::endl;
                  registered_node_collections_[node_collection] =
                      population_id.as_number().to_int64();
                });
          }
        })
        .wait();  // Wait because it may cause a race condition
  }
  node_collections_to_register_.clear();
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
    if (!binary_search(gids.begin(), gids.end(), sender_gid_)) {
      gids.insert(std::lower_bound(gids.begin(), gids.end(), sender_gid_), 
        sender_gid_);
      multimeter.needs_update = true;
    }

    for (std::size_t i = 0; i < double_values.size(); ++i)
      data_storage_.AddMultimeterMeasurement(device_id, 
        multimeter.double_attributes[i], time_stamp, sender_gid_, 
        double_values[i]);
    for (std::size_t i = 0; i < long_values.size(); ++i)
      data_storage_.AddMultimeterMeasurement(device_id, 
        multimeter.long_attributes[i], time_stamp, sender_gid_, 
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
