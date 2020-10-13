#include "http_server.hpp"

#include <algorithm>
#include <iostream>
#include <regex>
#include <unordered_set>

#include "storage/data_storage.hpp"
#include "nest_time.h"
#include "kernel_manager.h"

namespace insite {

HttpServer::HttpServer(web::http::uri address, DataStorage* storage,
                       std::string database_uri)
    : http_listener_{address},
      storage_(storage),
      database_uri_(database_uri) {
  http_listener_.support([this](web::http::http_request request) {
    if (request.method() == "GET" &&
        request.relative_uri().path() == "/kernelStatus") {
      request.reply(GetKernelStatus(request));
    } else if (request.method() == "GET" &&
        request.relative_uri().path() == "/nodeCollections") {
      request.reply(GetCollections(request));
    } else if (request.method() == "GET" &&
        request.relative_uri().path() == "/spikedetectors") {
      request.reply(GetSpikeDetectors(request));
    } else if (request.method() == "GET" &&
        request.relative_uri().path() == "/spikes") {
      request.reply(GetSpikes(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/multimeter_measurement") {
      request.reply(GetMultimeterMeasurement(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/simulation_time_info") {
      request.reply(GetCurrentSimulationTime(request));
    } else {
      request.reply(CreateErrorResponse(
          web::http::status_codes::NotFound,
          {"Invalid Endpoint", "The endpoint does not exists."}));
    }
  });

  http_listener_.open().wait();
  std::cout << "HTTP server is listening...\n";
}

web::http::http_response HttpServer::GetKernelStatus(const web::http::http_request& request) {

  DictionaryDatum kernel_status(new Dictionary());
  nest::kernel().get_status(kernel_status);

  for (const auto& datum : *kernel_status) {
    std::cout << datum.first << ": ";
    datum.second->print(std::cout);
    std::cout << std::endl;
  }

  try {
    web::http::http_response response;
    web::json::value response_body = web::json::value::object();
    response_body["resolution"] =
        getValue<double>(kernel_status->lookup2("resolution"));
    response_body["time"] = getValue<double>(kernel_status->lookup2("time"));
    response_body["to_do"] = getValue<long>(kernel_status->lookup2("to_do"));
    response_body["max_delay"] =
        getValue<double>(kernel_status->lookup2("max_delay"));
    response_body["min_delay"] =
        getValue<double>(kernel_status->lookup2("min_delay"));
    response_body["ms_per_tic"] =
        getValue<double>(kernel_status->lookup2("ms_per_tic"));
    response_body["tics_per_ms"] =
        getValue<double>(kernel_status->lookup2("tics_per_ms"));
    response_body["tics_per_step"] =
        getValue<long>(kernel_status->lookup2("tics_per_step"));
    response_body["T_max"] = getValue<double>(kernel_status->lookup2("T_max"));
    response_body["T_min"] = getValue<double>(kernel_status->lookup2("T_min"));
    response_body["total_num_virtual_procs"] =
        getValue<long>(kernel_status->lookup2("total_num_virtual_procs"));
    response_body["local_num_threads"] =
        getValue<long>(kernel_status->lookup2("local_num_threads"));
    response_body["num_processes"] =
        getValue<long>(kernel_status->lookup2("num_processes"));
    response_body["off_grid_spiking"] =
        getValue<bool>(kernel_status->lookup2("off_grid_spiking"));
    // response_body["initial_connector_capacity"] =
    //     getValue<long>(kernel_status->lookup2("initial_connector_capacity"));
    // response_body["large_connector_limit"] =
    //     getValue<long>(kernel_status->lookup2("large_connector_limit"));
    // response_body["large_connector_growth_factor"] = getValue<double>(
    //     kernel_status->lookup2("large_connector_growth_factor"));
    response_body["grng_seed"] =
        getValue<long>(kernel_status->lookup2("grng_seed"));
    // response_body["rng_seeds"] =
    //     getValue<array>(kernel_status->lookup2("rng_seeds"));
    // response_body["data_path"] =
    //     getValue<string>(kernel_status->lookup2("data_path"));
    // response_body["data_prefix"] =
    //     getValue<string>(kernel_status->lookup2("data_prefix"));
    response_body["overwrite_files"] =
        getValue<bool>(kernel_status->lookup2("overwrite_files"));
    response_body["print_time"] =
        getValue<bool>(kernel_status->lookup2("print_time"));
    response_body["network_size"] =
        getValue<long>(kernel_status->lookup2("network_size"));
    response_body["num_connections"] =
        getValue<long>(kernel_status->lookup2("num_connections"));
    response_body["use_wfr"] =
        getValue<bool>(kernel_status->lookup2("use_wfr"));
    response_body["wfr_comm_interval"] =
        getValue<double>(kernel_status->lookup2("wfr_comm_interval"));
    response_body["wfr_tol"] =
        getValue<double>(kernel_status->lookup2("wfr_tol"));
    response_body["wfr_max_iterations"] =
        getValue<long>(kernel_status->lookup2("wfr_max_iterations"));
    response_body["wfr_interpolation_order"] =
        getValue<long>(kernel_status->lookup2("wfr_interpolation_order"));
    response_body["dict_miss_is_error"] =
        getValue<bool>(kernel_status->lookup2("dict_miss_is_error"));

    response.set_status_code(web::http::status_codes::OK);
    response.set_body(response_body);
    return response;
  } catch (const UndefinedName& exception) {
    return CreateErrorResponse(web::http::status_codes::InternalError,
                               {"UndefinedName", exception.message()});
  } catch (...) {
    return CreateErrorResponse(web::http::status_codes::InternalError,
                               {"UnknownError"});
  }
}

web::http::http_response HttpServer::GetCurrentSimulationTime(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  
  web::json::value response_body = web::json::value::object();
  response_body["current"] = storage_->GetCurrentSimulationTime();
  response_body["begin"] = storage_->GetSimulationEndTime();
  response_body["end"] = storage_->GetSimulationBeginTime();
  response_body["step_size"] = nest::Time(nest::Time::step(1)).get_ms();
  response.set_body(response_body);
  return response;
}
  
web::http::http_response HttpServer::GetCollections(const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  
  const auto node_collections = storage_->GetNodeCollections();

  web::json::value response_body =  web::json::value::array(node_collections.size());
  for (size_t i = 0; i < node_collections.size(); ++i) {
    response_body[i] = web::json::value::object();
    response_body[i]["id"] = i;
    response_body[i]["firstNodeId"] = node_collections[i].first_node_id;
    response_body[i]["nodeCount"] = node_collections[i].node_count;
    response_body[i]["modelName"] = web::json::value(node_collections[i].model_name);
  }
  response.set_body(response_body);

  return response;
}
  
web::http::http_response HttpServer::GetSpikeDetectors(const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  
  const auto spike_detectors = storage_->GetSpikeDetectors();

  web::json::value response_body =  web::json::value::array();
  std::vector<std::uint64_t> connected_node_ids;

  for (const auto& spikedetector_id_storage : spike_detectors) {
    web::json::value spikedetector_data = web::json::value::object();
    spikedetector_data["id"] = spikedetector_id_storage.first;

    spikedetector_id_storage.second->ExtractConnectedNodeIds(&connected_node_ids);
    spikedetector_data["connectedNodes"] = web::json::value::array(connected_node_ids.size());
    for (size_t i = 0; i < connected_node_ids.size(); ++i) {
      spikedetector_data["connectedNodes"][i] = connected_node_ids[i];
    }

    response_body[response_body.size()] = spikedetector_data;
  }

  response.set_body(response_body);

  return response;
}

web::http::http_response HttpServer::GetSpikes(
    const web::http::http_request& request) {
  const auto parameters = web::uri::split_query(request.request_uri().query());

  web::http::http_response response(web::http::status_codes::OK);

  const auto from_time_parameter = parameters.find("fromTime");
  const double from_time = from_time_parameter == parameters.end()
                               ? 0.0
                               : std::stod(from_time_parameter->second);

  const auto to_time_parameter = parameters.find("toTime");
  const double to_time = to_time_parameter == parameters.end()
                             ? std::numeric_limits<double>::infinity()
                             : std::stod(to_time_parameter->second);

  const auto node_collection_parameter = parameters.find("nodeCollectionId");
  std::uint64_t from_node_id = 0;
  std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
  if (node_collection_parameter != parameters.end()) {
    const std::uint64_t node_collection_id = std::stoull(node_collection_parameter->second);
    if (node_collection_id < storage_->GetNodeCollectionCount()) {
      const NodeCollection node_collection = storage_->GetNodeCollection(node_collection_id);
      from_node_id = node_collection.first_node_id;
      to_node_id = node_collection.first_node_id + node_collection.node_count;
    } else {
      return CreateErrorResponse(web::http::status_codes::BadRequest,
                                 {"InvalidNodeCollectionID"});
    }
  }

  const auto spike_detector_id_parameter = parameters.find("spikedetectorId");
  
  std::vector<Spike> spikes;

  if (spike_detector_id_parameter == parameters.end()) {
  std::cout << "Querying spikes: [" << from_time << "," << to_time << " ["
            << from_node_id << "," << to_node_id << ")" << std::endl;
            
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>>
        spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes(
          &spikes, from_time, to_time, from_node_id, to_node_id);
    }

  } else {
    const auto spike_detector_id = std::stoll(spike_detector_id_parameter->second);
    const auto spike_detector = storage_->GetSpikeDetectorStorage(spike_detector_id);

    std::cout << "Querying spikes: [" << from_time << "," << to_time << " ["
              << from_node_id << "," << to_node_id
              << "), spikedetector=" << spike_detector_id << std::endl;

    if (spike_detector == nullptr) {
      return CreateErrorResponse(web::http::status_codes::BadRequest,
                                 {"InvalidSpikeDetectorId"});
    } else {
      spike_detector->ExtractSpikes(&spikes, from_time, to_time, from_node_id, to_node_id);
    }
  }

  web::json::value node_ids = web::json::value::array(spikes.size());
  web::json::value simulation_times = web::json::value::array(spikes.size());

  {
    size_t index = 0;
    for (const auto& spike : spikes) {
      node_ids[index] = spike.node_id;
      simulation_times[index] = spike.simulation_time;
      index++;
    }
  }

  response.set_body(web::json::value::object(
      {{"simulationTimes", simulation_times}, {"nodeIds", node_ids}}));

  // const auto spike_happened_before = [](const Spike& spike,
  //                                       double simulation_time) {
  //   return spike.simulation_time < simulation_time;
  // };


  return response;
}

web::http::http_response HttpServer::GetMultimeterMeasurement(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  web::json::value body = web::json::value::object();

  const auto parameters = web::uri::split_query(request.request_uri().query());
  const auto parameter_multimeter_id = parameters.find("multimeter_id");
  const auto parameter_attribute = parameters.find("attribute");
  const auto parameter_from = parameters.find("from");
  const auto parameter_to = parameters.find("to");
  const auto parameter_gids = parameters.find("gids");
  const auto parameter_offset = parameters.find("offset");
  const auto parameter_limit = parameters.find("limit");

  const auto multimeter_id = std::stoll(parameter_multimeter_id->second);
  const auto attribute = parameter_attribute->second;

  auto filter_gids = std::vector<std::uint64_t>();
  if (parameter_gids != parameters.end()) {
    std::regex regex{R"([\s,]+)"};
    std::sregex_token_iterator it{parameter_gids->second.begin(),
                                  parameter_gids->second.end(), regex, -1};
    std::vector<std::string> filter_gid_strings{it, {}};
    std::transform(filter_gid_strings.begin(), filter_gid_strings.end(),
                   std::back_inserter(filter_gids),
                   [](const std::string& str) { return std::stoll(str); });
  }
  std::cout << "Filter GID count: " << filter_gids.size() << "\n";

  const auto measurements = storage_->GetMultimeterMeasurements();
  if (measurements.find(multimeter_id) != measurements.end() &&
      measurements.at(multimeter_id).find(attribute) !=
          measurements.at(multimeter_id).end()) {
    auto& measurement = measurements.at(multimeter_id).at(attribute);
    auto& simulation_times = measurement.simulation_times;
    auto& gids = measurement.gids;
    auto& values = measurement.values;

    auto simulation_times_begin =
        parameter_from == parameters.end()
            ? simulation_times.begin()
            : std::lower_bound(simulation_times.begin(), simulation_times.end(),
                               std::stoll(parameter_from->second));
    auto simulation_times_end =
        parameter_to == parameters.end()
            ? simulation_times.end()
            : std::lower_bound(simulation_times.begin(), simulation_times.end(),
                               std::stoll(parameter_to->second));
    if (parameter_offset != parameters.end())
      simulation_times_begin += std::stoll(parameter_offset->second);
    if (parameter_limit != parameters.end())
      simulation_times_end =
          simulation_times_begin + std::stoll(parameter_limit->second);
    auto simulation_times_subset = std::vector<web::json::value>(
        simulation_times_begin, simulation_times_end);
    std::size_t simulation_start_index =
        std::distance(simulation_times.begin(), simulation_times_begin);
    std::size_t simulation_end_index =
        std::distance(simulation_times.begin(), simulation_times_end);

    auto gids_begin = filter_gids.empty() ? gids.begin() : filter_gids.begin();
    auto gids_end = filter_gids.empty() ? gids.end() : filter_gids.end();
    auto gids_subset = std::vector<web::json::value>(gids_begin, gids_end);
    auto gid_indices = std::vector<std::size_t>();
    if (!filter_gids.empty())
      for (auto& filter_gid : filter_gids)
        gid_indices.push_back(std::distance(
            gids.begin(),
            std::lower_bound(gids.begin(), gids.end(), filter_gid)));
    else {
      gid_indices.resize(gids.size());
      std::iota(gid_indices.begin(), gid_indices.end(), 0);
    }

    auto values_subset = std::vector<web::json::value>(
        simulation_times_subset.size() * gids_subset.size());
    for (std::size_t t = 0, vt = simulation_start_index;
         vt < simulation_end_index; ++t, ++vt)
      for (std::size_t g = 0; g < gid_indices.size(); ++g)
        values_subset[t * gids_subset.size() + g] =
            values[vt * gids.size() + gid_indices[g]];

    body["simulation_times"] = web::json::value::array(simulation_times_subset);
    body["gids"] = web::json::value::array(gids_subset);
    body["values"] = web::json::value::array(values_subset);
  }

  response.set_body(body);
  return response;
}

web::json::value HttpServer::Error::Serialize() const {
  web::json::value error = web::json::value::object();
  error["code"] = web::json::value(code);
  error["message"] = web::json::value(message);
  return error;
}

web::http::http_response HttpServer::CreateErrorResponse(web::http::status_code status_code, const Error& error) {
  web::http::http_response response(status_code);

  web::json::value body = web::json::value::object();
  body["error"] = error.Serialize();
  response.set_body(body);

  return response;
}

}  // namespace insite
