// #include <cpprest/http_msg.h>
#include <crow.h>
#include "crow/query_string.h"
#include "extern/tartanllama/optional.h"
#include "extern/websocketpp/websocketpp/frame.hpp"
#include "json_serializer.h"
#include "parameters.h"
#include "stopwatch_helper.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <crow/app.h>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include "http_server.hpp"
// #include "extern/websocketpp/websocketpp/roles/server_endpoint.hpp"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <unistd.h>
#include "flatbuffers/flatbuffer_builder.h"
#include "kernel_manager.h"
#include "nest_time.h"
#include "schema_generated.h"
#include "serialize.hpp"
#include "storage/data_storage.hpp"
#include "storage/spikedetector_storage.hpp"
// #define ASIO_STANDALONE
// #define _WEBSOCKETPP_CPP11_STRICT_
// #include <websocketpp/config/asio_no_tls.hpp>
// #include <websocketpp/client.hpp>
/* #include "rapidjson/" */
#define CROW_ROUTE_LAMBA(APP, ENDPOINT, FUNCTION) CROW_ROUTE(APP, ENDPOINT)([this](const crow::request& request) { return FUNCTION(request); });

namespace flatbuffers {

static Test::Foo::Spikes32 Pack32(const insite::Spike32& spike) {
  return Test::Foo::Spikes32{spike.simulation_time, spike.node_id};
};

static Test::Foo::Spikes Pack(const insite::Spike& spike) {
  return Test::Foo::Spikes{spike.simulation_time, spike.node_id};
};
}  // namespace flatbuffers

namespace insite {

std::map<int, int> HttpServer::run_no;
int HttpServer::user_id = 0;
bool HttpServer::prepare_finished = false;

HttpServer::HttpServer(std::string address, DataStorage* storage)
    : storage_(storage) {
  // websocketpp::server
  CROW_ROUTE_LAMBA(app, "/spikesfb", GetSpikesFB)
  CROW_ROUTE_LAMBA(app, "/spikesws", GetSpikesSendWS)
  CROW_ROUTE_LAMBA(app, "/version", GetVersion)

  CROW_ROUTE_LAMBA(app, "/spikes", GetSpikes)
  CROW_ROUTE_LAMBA(app, "/v2/spikes", GetSpikesV2)

  CROW_ROUTE_LAMBA(app, "/simulationTimeInfo", GetCurrentSimulationTime)
  CROW_ROUTE_LAMBA(app, "/v2/simulationTimeInfo", GetCurrentSimulationTimeV2)

  CROW_ROUTE_LAMBA(app, "/kernelStatus", GetKernelStatus)
  CROW_ROUTE_LAMBA(app, "/v2/kernelStatus", GetKernelStatusV2)

  CROW_ROUTE_LAMBA(app, "/multimeters", GetMultimeters)
  CROW_ROUTE_LAMBA(app, "/v2/multimeters", GetMultimetersV2)

  CROW_ROUTE_LAMBA(app, "/multimeter_measurement", GetMultimeterMeasurement)
  CROW_ROUTE_LAMBA(app, "/v2/multimeter_measurement", GetMultimeterMeasurementV2)

  CROW_ROUTE_LAMBA(app, "/nodeCollections", GetCollections)
  CROW_ROUTE_LAMBA(app, "/v2/nodeCollections", GetCollectionsV2)

  CROW_ROUTE_LAMBA(app, "/nodes", GetNodes)
  CROW_ROUTE_LAMBA(app, "/v2/nodes", GetNodesV2)

  CROW_ROUTE_LAMBA(app, "/spikerecorders", GetSpikeRecorders)
  CROW_ROUTE_LAMBA(app, "/v2/spikerecorders", GetSpikeRecordersV2)

  app.stream_threshold(std::numeric_limits<unsigned int>::max());
  crow_server = app.port(18080 + nest::kernel().mpi_manager.get_rank()).multithreaded().run_async();
  app.wait_for_server_start();

  SPDLOG_INFO("[insite] http server is listening...");
}

void HttpServer::ClearSimulationHasEnded() {
  simulation_end_time_ = -1;
}

void HttpServer::SimulationHasEnded(double end_time_) {
  simulation_end_time_ = end_time_;
}

crow::response HttpServer::GetVersion(const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  json_serializer::Version(writer);
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetCurrentSimulationTime(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  json_serializer::SimulationTimeInfo(writer, storage_->GetSimulationTimeInfo());
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetCurrentSimulationTimeV2(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  if (prepare_finished == false) {
    return crow::response(crow::status::ACCEPTED);
  }

  json_serializer::SimulationTimeInfoV2(writer, storage_->GetSimulationTimeInfo());
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetKernelStatus(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  storage_->GetKernelStatus(writer);

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetKernelStatusV2(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  auto sim_id = std::to_string(HttpServer::user_id) + ":" + std::to_string(HttpServer::run_no[user_id]);

  writer.StartObject();
  writer.Key("simId");
  writer.String(sim_id.c_str());

  writer.Key("kernelStatus");
  storage_->GetKernelStatus(writer);
  writer.EndObject();

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetCollectionsV2(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  const auto node_collections = storage_->GetNodeCollections();
  json_serializer::NodeCollectionsV2(writer, node_collections);
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetCollections(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  const auto node_collections = storage_->GetNodeCollections();
  json_serializer::NodeCollections(writer, node_collections);
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetNodes(
    const crow::request& request) {
  std::unordered_map<uint64_t, std::string> nodes = storage_->GetNodes();

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartArray();
  for (const auto& node : nodes) {
    writer.RawValue(node.second.c_str(), node.second.length(), rapidjson::kObjectType);
  }
  writer.EndArray();
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetNodesV2(
    const crow::request& request) {
  std::unordered_map<uint64_t, std::string> nodes = storage_->GetNodes();

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  auto sim_id = std::to_string(HttpServer::user_id) + ":" + std::to_string(HttpServer::run_no[user_id]);

  writer.StartObject();
  writer.Key("simId");
  writer.String(sim_id.c_str());
  writer.Key("nodes");
  writer.StartArray();
  for (const auto& node : nodes) {
    writer.RawValue(node.second.c_str(), node.second.length(), rapidjson::kObjectType);
  }
  writer.EndArray();
  writer.EndObject();
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetSpikeRecorders(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  const auto spike_detectors = storage_->GetSpikeDetectors();
  json_serializer::SpikeDetectors(writer, spike_detectors);

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetSpikeRecordersV2(
    const crow::request& request) {
  if (!prepare_finished) {
    auto response = crow::response();
    response.code = crow::status::ACCEPTED;
    return response;
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  const auto spike_detectors = storage_->GetSpikeDetectors();
  json_serializer::SpikeDetectorsV2(writer, spike_detectors);

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetSpikesFB(const crow::request& request) {
  std::unordered_map<std::string, std::string> parameters;

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
  auto parseString = (parameter_gids == parameters.end()) ? std::string("") : parameter_gids->second;
  auto filter_node_ids = CommaListToUintVector(parseString);

  std::uint64_t from_node_id = 0;
  std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
  if (!filter_node_ids.empty()) {
    from_node_id = *std::min_element(filter_node_ids.begin(), filter_node_ids.end());
    to_node_id = *std::max_element(filter_node_ids.begin(), filter_node_ids.end());
  }
  if (node_collection_parameter != parameters.end()) {
    const std::uint64_t node_collection_id =
        std::stoull(node_collection_parameter->second);
    if (node_collection_id < storage_->GetNodeCollectionCount()) {
      const NodeCollection node_collection =
          storage_->GetNodeCollection(node_collection_id);
      from_node_id = node_collection.first_node_id;
      to_node_id = node_collection.first_node_id + node_collection.node_count - 1;
    } else {
      return crow::response("InvalidNodeCollectionID");
    }
  }

  const auto spike_detector_id_parameter = parameters.find("spikedetectorId");

  std::vector<Spike> spikes;
  spikes.reserve(40000000);

  if (spike_detector_id_parameter == parameters.end()) {
    std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id << ")]" << std::endl;
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes2(&spikes, from_time, to_time, from_node_id, to_node_id, &filter_node_ids);
    }

  } else {
    const auto spike_detector_id =
        std::stoll(spike_detector_id_parameter->second);
    const auto spike_detector =
        storage_->GetSpikeDetectorStorage(spike_detector_id);

    std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id
              << ")], spikedetector=" << spike_detector_id << std::endl;

    if (spike_detector == nullptr) {
      return crow::response("InvalidSpikeDetectorId");
    } else {
      spike_detector->ExtractSpikes2(&spikes, from_time, to_time, from_node_id,
                                     to_node_id, &filter_node_ids);
    }
  }

  bool last_frame = simulation_end_time_ != -1 && (to_time >= simulation_end_time_);
  flatbuffers::FlatBufferBuilder fbb(40000000 * 16);
  auto fb_spikes = fbb.CreateVectorOfNativeStructs<Test::Foo::Spikes>(spikes, flatbuffers::Pack);
  auto fb_spike_table = Test::Foo::CreateSpikyTable(fbb, fb_spikes);
  fbb.Finish(fb_spike_table);
  return crow::response(std::string((const char*)fbb.GetBufferPointer(), fbb.GetSize()));
}

crow::response HttpServer::GetSpikesSendWS(const crow::request& request) {
  StopwatchHelper swh;

  SpikeParameter params(request.url_params);

  swh.checkpoint("Parameter parsing");
  std::uint64_t from_node_id = 0;
  std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
  if (not params.node_gids.empty()) {
    from_node_id = *std::min_element(params.node_gids.begin(), params.node_gids.end());
    to_node_id = *std::max_element(params.node_gids.begin(), params.node_gids.end());
  }

  swh.reset();
  if (params.node_collection_id) {
    if (params.node_collection_id >= storage_->GetNodeCollectionCount()) {
      return crow::response("InvalidNodeCollectionID");
    }

    const NodeCollection node_collection =
        storage_->GetNodeCollection(params.node_collection_id.value());
    from_node_id = node_collection.first_node_id;
    to_node_id = node_collection.first_node_id + node_collection.node_count - 1;
  }

  std::vector<Spike> spikes;

  if (params.spike_detector_id) {
    const auto spike_detector =
        storage_->GetSpikeDetectorStorage(params.spike_detector_id.value());
    if (spike_detector == nullptr) {
      return crow::response("InvalidSpikeDetectorId");
    }

    SPDLOG_TRACE("[insite] Querying spikes: [time: ( {} - {}) ] [nodes: ({} - {})] spikedetector = %i", params.from_time.value(), params.to_time.value(), from_node_id, to_node_id, params.spike_detector_id.value());
    spike_detector->ExtractSpikes(&spikes, params.from_time.value(), params.to_time.value(), from_node_id,
                                  to_node_id, &params.node_gids);
  } else {
    SPDLOG_TRACE("[insite] Querying spikes: [time: ( {} - {}) ] [nodes: ({} - {})]", params.from_time.value(), params.to_time.value(), from_node_id, to_node_id);
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes(&spikes, params.from_time.value(), params.to_time.value(), from_node_id, to_node_id, &params.node_gids);
    }
  }

  swh.checkpoint("Extracting Spikes");

  bool last_frame = simulation_end_time_ != -1 && (params.to_time >= simulation_end_time_);
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  json_serializer::Spikes(writer, spikes, last_frame);
  swh.checkpoint("Serializing Spikes");
  con->send(s.GetString(), s.GetLength(), websocketpp::frame::opcode::BINARY);
  swh.checkpoint("Sending Spikes");
  swh.print();
  return crow::response("ok");
}

crow::response HttpServer::GetSpikesV2(const crow::request& request) {
  StopwatchHelper swh;

  SpikeParameter params(request.url_params);

  swh.checkpoint("Parameter parsing");
  std::uint64_t from_node_id = 0;
  std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
  if (not params.node_gids.empty()) {
    from_node_id = *std::min_element(params.node_gids.begin(), params.node_gids.end());
    to_node_id = *std::max_element(params.node_gids.begin(), params.node_gids.end());
  }

  swh.reset();
  if (params.node_collection_id) {
    if (params.node_collection_id >= storage_->GetNodeCollectionCount()) {
      return crow::response("InvalidNodeCollectionID");
    }

    const NodeCollection node_collection =
        storage_->GetNodeCollection(params.node_collection_id.value());
    from_node_id = node_collection.first_node_id;
    to_node_id = node_collection.first_node_id + node_collection.node_count - 1;
  }

  std::vector<Spike> spikes;

  if (params.spike_detector_id) {
    const auto spike_detector =
        storage_->GetSpikeDetectorStorage(params.spike_detector_id.value());
    if (spike_detector == nullptr) {
      return crow::response("InvalidSpikeDetectorId");
    }

    SPDLOG_TRACE("[insite] Querying spikes: [time: ( {} - {}) ] [nodes: ({} - {})] spikedetector = %i", params.from_time.value(), params.to_time.value(), from_node_id, to_node_id, params.spike_detector_id.value());

    spike_detector->ExtractSpikes(&spikes, params.from_time.value(), params.to_time.value(), from_node_id,
                                  to_node_id, &params.node_gids);
  } else {
    SPDLOG_TRACE("[insite] Querying spikes: [time: ( {} - {}) ] [nodes: ({} - {})]", params.from_time.value(), params.to_time.value(), from_node_id, to_node_id);
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes(&spikes, params.from_time.value(), params.to_time.value(), from_node_id, to_node_id, &params.node_gids);
    }
  }

  swh.checkpoint("Extracting Spikes");

  bool last_frame = simulation_end_time_ != -1 && (params.to_time >= simulation_end_time_);
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  json_serializer::SpikesV2(writer, spikes, last_frame);
  swh.checkpoint("Serializing Spikes");
  swh.print();
  return crow::response(s.GetString());
}

crow::response HttpServer::GetSpikes(const crow::request& request) {
  StopwatchHelper swh;

  SpikeParameter params(request.url_params);

  swh.checkpoint("Parameter parsing");
  std::uint64_t from_node_id = 0;
  std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
  if (not params.node_gids.empty()) {
    from_node_id = *std::min_element(params.node_gids.begin(), params.node_gids.end());
    to_node_id = *std::max_element(params.node_gids.begin(), params.node_gids.end());
  }

  swh.reset();
  if (params.node_collection_id) {
    if (params.node_collection_id >= storage_->GetNodeCollectionCount()) {
      return crow::response("InvalidNodeCollectionID");
    }

    const NodeCollection node_collection =
        storage_->GetNodeCollection(params.node_collection_id.value());
    from_node_id = node_collection.first_node_id;
    to_node_id = node_collection.first_node_id + node_collection.node_count - 1;
  }

  std::vector<Spike> spikes;

  if (params.spike_detector_id) {
    const auto spike_detector =
        storage_->GetSpikeDetectorStorage(params.spike_detector_id.value());
    if (spike_detector == nullptr) {
      return crow::response("InvalidSpikeDetectorId");
    }

    SPDLOG_TRACE("[insite] Querying spikes: [time: ( {} - {}) ] [nodes: ({} - {})] spikedetector = %i", params.from_time.value(), params.to_time.value(), from_node_id, to_node_id, params.spike_detector_id.value());

    spike_detector->ExtractSpikes(&spikes, params.from_time.value(), params.to_time.value(), from_node_id,
                                  to_node_id, &params.node_gids);
  } else {
    SPDLOG_TRACE("[insite] Querying spikes: [time: ( {} - {}) ] [nodes: ({} - {})]", params.from_time.value(), params.to_time.value(), from_node_id, to_node_id);
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes(&spikes, params.from_time.value(), params.to_time.value(), from_node_id, to_node_id, &params.node_gids);
    }
  }

  swh.checkpoint("Extracting Spikes");

  bool last_frame = simulation_end_time_ != -1 && (params.to_time >= simulation_end_time_);
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  json_serializer::Spikes(writer, spikes, last_frame);
  swh.checkpoint("Serializing Spikes");
  swh.print();
  return crow::response(s.GetString());
}

crow::response HttpServer::GetMultimetersV2(
    const crow::request& request) {
  const auto multimeters = storage_->GetMultimeters();

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  json_serializer::MultimetersV2(writer, multimeters);

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetMultimeters(
    const crow::request& request) {
  const auto multimeters = storage_->GetMultimeters();
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  json_serializer::Multimeters(writer, multimeters);

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetMultimeterMeasurement(
    const crow::request& request) {
  MultimeterParameter params(request.url_params);

  SPDLOG_TRACE("filter nodes: {}", params.node_gids);

  if (not params.attribute.has_value()) {
    return crow::response("The 'attributeName' parameter is missing from the request.");
  }

  if (not params.multimeter_id) {
    return crow::response("The 'multimeterId' parameter is missing from the request.");
  }

  const auto multimeters = storage_->GetMultimeters();
  const auto multimeter = multimeters.find(params.multimeter_id.value());
  if (multimeter == multimeters.end()) {
    return crow::response("InvalidMultimeterId");
  }

  rapidjson::StringBuffer s;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

  SPDLOG_TRACE("Getting MM Measurements {} for {} from {} to {}", params.attribute.value(), multimeter->second->id_, params.from_time.value(), params.to_time.value());
  multimeter->second->ExtractMeasurements(writer, params.attribute.value(), params.node_gids, params.from_time.value(), params.to_time.value());
  return crow::response(s.GetString());
}

crow::response HttpServer::GetMultimeterMeasurementV2(
    const crow::request& request) {
  MultimeterParameter params(request.url_params);

  SPDLOG_TRACE("filter nodes: {}", params.node_gids);

  if (not params.attribute.has_value()) {
    return crow::response("The 'attributeName' parameter is missing from the request.");
  }

  if (not params.multimeter_id) {
    return crow::response("The 'multimeterId' parameter is missing from the request.");
  }

  const auto multimeters = storage_->GetMultimeters();
  const auto multimeter = multimeters.find(params.multimeter_id.value());
  if (multimeter == multimeters.end()) {
    return crow::response("InvalidMultimeterId");
  }

  rapidjson::StringBuffer s;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

  SPDLOG_TRACE("Getting MM Measurements {} for {} from {} to {}", params.attribute.value(), multimeter->second->id_, params.from_time.value(), params.to_time.value());
  multimeter->second->ExtractMeasurementsV2(writer, params.attribute.value(), params.node_gids, params.from_time.value(), params.to_time.value());
  return crow::response(s.GetString());
}

std::vector<std::uint64_t> HttpServer::CommaListToUintVector(std::string input,
                                                             std::regex regex) {
  std::vector<std::uint64_t> filter_node_ids;
  if (input == "")
    return filter_node_ids;
  std::sregex_token_iterator it{input.begin(), input.end(), regex, -1};
  std::vector<std::string> filter_gid_strings{it, {}};
  std::transform(filter_gid_strings.begin(), filter_gid_strings.end(),
                 std::back_inserter(filter_node_ids),
                 [](const std::string& str) { return std::stoll(str); });
  return filter_node_ids;
}

}  // namespace insite
