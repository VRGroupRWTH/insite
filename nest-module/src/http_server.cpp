#include "http_server.hpp"

#include <algorithm>
#include <iostream>
#include <regex>
#include <unordered_set>

#include "kernel_manager.h"
#include "nest_time.h"
#include "serialize.hpp"
#include "storage/data_storage.hpp"

namespace insite {

HttpServer::HttpServer(web::http::uri address, DataStorage* storage)
    : http_listener_{address}, storage_(storage) {
  http_listener_.support([this](web::http::http_request request) {
    if (request.method() == "GET" &&
        request.relative_uri().path() == "/version") {
      request.reply(GetVersion(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/kernelStatus") {
      request.reply(GetKernelStatus(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/nodeCollections") {
      request.reply(GetCollections(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/nodes") {
      request.reply(GetNodes(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/spikedetectors") {
      request.reply(GetSpikeDetectors(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/spikes") {
      request.reply(GetSpikes(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/multimeters") {
      request.reply(GetMultimeters(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/multimeter_measurement") {
      request.reply(GetMultimeterMeasurement(request));
    } else if (request.method() == "GET" &&
               request.relative_uri().path() == "/simulationTimeInfo") {
      request.reply(GetCurrentSimulationTime(request));
    } else {
      request.reply(CreateErrorResponse(
          web::http::status_codes::NotFound,
          {"Invalid Endpoint", "The endpoint does not exists."}));
    }
  });

  http_listener_.open().wait();
  std::cout << "[insite] HTTP server is listening...\n";
}

web::http::http_response HttpServer::GetVersion(
    const web::http::http_request& request) {
  DictionaryDatum kernel_status(new Dictionary());
  nest::kernel().get_status(kernel_status);

  web::http::http_response response(web::http::status_codes::OK);
  response.set_body(web::json::value::string("1.0"));

  return response;
}

web::http::http_response HttpServer::GetKernelStatus(
    const web::http::http_request& request) {
  DictionaryDatum kernel_status(new Dictionary());
  nest::kernel().get_status(kernel_status);

  web::http::http_response response(web::http::status_codes::OK);
  response.set_body(SerializeDatum(&kernel_status));

  return response;
}

web::http::http_response HttpServer::GetCurrentSimulationTime(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);

  web::json::value response_body = web::json::value::object();
  response_body["current"] = storage_->GetCurrentSimulationTime();
  response_body["begin"] = storage_->GetSimulationEndTime();
  response_body["end"] = storage_->GetSimulationBeginTime();
  response_body["stepSize"] = nest::Time(nest::Time::step(1)).get_ms();
  response.set_body(response_body);
  return response;
}

web::http::http_response HttpServer::GetCollections(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);

  const auto node_collections = storage_->GetNodeCollections();

  web::json::value response_body =
      web::json::value::array(node_collections.size());
  for (size_t i = 0; i < node_collections.size(); ++i) {
    response_body[i] = web::json::value::object();
    response_body[i]["nodeCollectionId"] = i;

    auto nodes = web::json::value::object();
    nodes["firstId"] = node_collections[i].first_node_id;
    nodes["lastId"] =
        node_collections[i].first_node_id + node_collections[i].node_count - 1;
    nodes["count"] = node_collections[i].node_count;
    response_body[i]["nodes"] = nodes;

    auto model = web::json::value::object();
    model["name"] = web::json::value(node_collections[i].model_name);

    auto model_parameters = web::json::value::array();
    for (const auto& model_parameter : node_collections[i].model_parameters) {
      model_parameters[model_parameters.size()] =
          web::json::value(model_parameter);
    }
    model["parameters"] = model_parameters;

    response_body[i]["model"] = model;
  }
  response.set_body(response_body);

  return response;
}

web::http::http_response HttpServer::GetNodes(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);

  const auto parameters = web::uri::split_query(request.request_uri().query());
  const auto local_only_parameter = parameters.find("localOnly");
  const bool local_only = local_only_parameter == parameters.end()
                              ? false
                              : (local_only_parameter->second == "false" ||
                                 local_only_parameter->second == "0");

  std::unordered_map<uint64_t, web::json::value> nodes = storage_->GetNodes();
  web::json::value response_body = web::json::value::array(nodes.size());

  unsigned int current_node_index = 0;
  for (const auto& node : nodes) {
    response_body[current_node_index] = node.second;
    ++current_node_index;
  }

  // DictionaryDatum node_properties(new Dictionary());

  // for (const nest::NodeIDTriple& node_id_triple : *nodes.get()) {
  //   nest::Node* node =
  //       nest::kernel().node_manager.get_node_or_proxy(node_id_triple.node_id);

  //   // auto model = web::json::value::object();
  //   // model["name"] = web::json::value(node->get_name());
  //   // node->get_status(node_properties);
  //   // model["parameters"] = SerializeDatum(&node_properties);

  //   auto serialized_node = web::json::value::object();

  //   serialized_node["nodeId"] = node_id_triple.node_id;
  //   serialized_node["nodeCollectionId"] = 0;
  //   serialized_node["position"] = 0;
  //   // serialized_node["model"] = model;

  //   response_body[current_node] = serialized_node;
  //   ++current_node;
  // }

  // for (size_t i = 0; i < node_collections.size(); ++i) {
  //   for (size_t j = 0; j < node_collections[i].node_count; ++j) {
  //     auto node = web::json::value::object();

  //     node["nodeId"] = node_collections[i].first_node_id + j;
  //     node["nodeCollectionId"] = i;
  //     node["position"] =

  //     response_body[current_node] = node;
  //   }

  //   auto nodes = web::json::value::object();
  //   nodes["firstId"] = node_collections[i].first_node_id;
  //   nodes["lastId"] = node_collections[i].first_node_id +
  //   node_collections[i].node_count - 1; nodes["count"] =
  //   node_collections[i].node_count; response_body[i]["nodes"] = nodes;

  //   auto model_parameters = web::json::value::array();
  //   for (const auto& model_parameter :
  //   node_collections[i].model_parameters) {
  //     model_parameters[model_parameters.size()] =
  //     web::json::value(model_parameter);
  //   }
  //   model["parameters"] = model_parameters;

  //   response_body[i]["model"] = model;
  // }
  response.set_body(response_body);

  return response;
}

web::http::http_response HttpServer::GetSpikeDetectors(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);

  const auto spike_detectors = storage_->GetSpikeDetectors();

  web::json::value response_body = web::json::value::array();
  std::vector<std::uint64_t> connected_node_ids;

  for (const auto& spikedetector_id_storage : spike_detectors) {
    web::json::value spikedetector_data = web::json::value::object();
    spikedetector_data["spikedetectorId"] = spikedetector_id_storage.first;

    spikedetector_id_storage.second->ExtractConnectedNodeIds(
        &connected_node_ids);
    spikedetector_data["nodeIds"] =
        web::json::value::array(connected_node_ids.size());
    for (size_t i = 0; i < connected_node_ids.size(); ++i) {
      spikedetector_data["nodeIds"][i] = connected_node_ids[i];
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

  const auto parameter_gids = parameters.find("nodeIds");
  auto filter_gids = commaListToUintVector(parameter_gids->second);


  std::uint64_t from_node_id = 0;
  std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
  if (!filter_gids.empty())
  {
    from_node_id = *std::min_element(filter_gids.begin(),filter_gids.end());
    to_node_id = *std::max_element(filter_gids.begin(),filter_gids.end());
  }
  if (node_collection_parameter != parameters.end()) {
    const std::uint64_t node_collection_id =
        std::stoull(node_collection_parameter->second);
    if (node_collection_id < storage_->GetNodeCollectionCount()) {
      const NodeCollection node_collection =
          storage_->GetNodeCollection(node_collection_id);
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
    std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id << ")]" << std::endl;
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes(&spikes, from_time, to_time, from_node_id, to_node_id, &filter_gids);
    }

  } else {
    const auto spike_detector_id =
        std::stoll(spike_detector_id_parameter->second);
    const auto spike_detector =
        storage_->GetSpikeDetectorStorage(spike_detector_id);

    std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id
              << ")], spikedetector=" << spike_detector_id << std::endl;

    if (spike_detector == nullptr) {
      return CreateErrorResponse(web::http::status_codes::BadRequest,
                                 {"InvalidSpikeDetectorId"});
    } else {
      spike_detector->ExtractSpikes(&spikes, from_time, to_time, from_node_id,
                                    to_node_id);
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

web::http::http_response HttpServer::GetMultimeters(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);

  const auto spike_detectors = storage_->GetSpikeDetectors();

  web::json::value response_body = web::json::value::array();
  std::vector<std::uint64_t> connected_node_ids;

  for (const auto& spikedetector_id_storage : spike_detectors) {
    web::json::value spikedetector_data = web::json::value::object();
    spikedetector_data["spikedetectorId"] = spikedetector_id_storage.first;

    spikedetector_id_storage.second->ExtractConnectedNodeIds(
        &connected_node_ids);
    spikedetector_data["nodeIds"] =
        web::json::value::array(connected_node_ids.size());
    for (size_t i = 0; i < connected_node_ids.size(); ++i) {
      spikedetector_data["nodeIds"][i] = connected_node_ids[i];
    }

    response_body[response_body.size()] = spikedetector_data;
  }

  response.set_body(response_body);

  return response;
}

web::http::http_response HttpServer::GetMultimeterMeasurement(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  web::json::value body = web::json::value::object();

  response.set_body(body);
  return response;
}

web::json::value HttpServer::Error::Serialize() const {
  web::json::value error = web::json::value::object();
  error["code"] = web::json::value(code);
  error["message"] = web::json::value(message);
  return error;
}

web::http::http_response HttpServer::CreateErrorResponse(
    web::http::status_code status_code, const Error& error) {
  web::http::http_response response(status_code);

  web::json::value body = web::json::value::object();
  body["error"] = error.Serialize();
  response.set_body(body);

  return response;
}
std::vector<std::uint64_t> HttpServer::commaListToUintVector(std::string input,
                                                    std::regex regex ) {
    std::vector<std::uint64_t> filter_gids;
    std::sregex_token_iterator it{input.begin(),
                                  input.end(), regex, -1};
    std::vector<std::string> filter_gid_strings{it, {}};
    std::transform(filter_gid_strings.begin(), filter_gid_strings.end(),
        std::back_inserter(filter_gids),
        [](const std::string& str) { return std::stoll(str); });
    return filter_gids;

}

}  // namespace insite
