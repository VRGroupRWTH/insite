// #include <cpprest/http_msg.h>
#include <crow.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

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

static Test::Foo::Spikes Pack(const insite::Spike& spike) {
  return Test::Foo::Spikes{spike.simulation_time, spike.node_id};
};
}  // namespace flatbuffers

namespace insite {
// typedef websocketpp::server<websocketpp::config::asio> server;
// void on_message(websocketpp::connection_hdl, server::message_ptr msg) {
//         std::cout << msg->get_payload() << std::endl;
// }



HttpServer::HttpServer(std::string address, DataStorage* storage)
    : storage_(storage) {
  // websocketpp::server
  CROW_ROUTE_LAMBA(app, "/spikesfb", GetSpikesFB)
  CROW_ROUTE_LAMBA(app, "/spikes", GetSpikes)
  CROW_ROUTE_LAMBA(app, "/version", GetVersion)
  CROW_ROUTE_LAMBA(app, "/simulationTimeInfo", GetCurrentSimulationTime)
  CROW_ROUTE_LAMBA(app, "/kernelStatus", GetKernelStatus)
  CROW_ROUTE_LAMBA(app, "/multimeters", GetMultimeters)
  CROW_ROUTE_LAMBA(app, "/multimeter_measurement", GetMultimeterMeasurement)
  CROW_ROUTE_LAMBA(app, "/nodeCollections", GetCollections)
  CROW_ROUTE_LAMBA(app, "/nodes", GetNodes)
  CROW_ROUTE_LAMBA(app, "/spikerecorders", GetSpikeRecorders)

  app.stream_threshold(std::numeric_limits<unsigned int>::max());
  crow_server = app.port(18080 + nest::kernel().mpi_manager.get_rank()).multithreaded().run_async();
  app.wait_for_server_start();

  spdlog::info("[insite] http server is listening...");
}

// httpserver::httpserver(web::http::uri address, datastorage* storage)
//     : http_listener_{address}, storage_(storage) {
//   http_listener_.support([this](web::http::http_request request) {
//     std::cout << "incoming request: " << request.request_uri().to_string() << std::endl;
//
//     if (request.method() == "get" &&
//         request.relative_uri().path() == "/version") {
//       request.reply(getversion(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/kernelstatus") {
//       request.reply(getkernelstatus(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/nodecollections") {
//       request.reply(getcollections(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/nodes") {
//       request.reply(getnodes(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/spikedetectors") {
//       request.reply(getspikedetectors(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/spikerecorders") {
//       request.reply(getspikerecorders(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/spikes") {
//       request.reply(getspikes(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/multimeters") {
//       request.reply(getmultimeters(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/multimeter_measurement") {
//       request.reply(getmultimetermeasurement(request));
//     } else if (request.method() == "get" &&
//                request.relative_uri().path() == "/simulationtimeinfo") {
//       request.reply(getcurrentsimulationtime(request));
//     } else {
//       request.reply(createerrorresponse(
//           web::http::status_codes::notfound,
//           {"invalid endpoint", "the endpoint does not exists."}));
//     }
//   });
//
//   // websocketpp::server
//   http_listener_.open().wait();
//   crow_route(app, "/spikesfb")
//   ([this](const crow::request& request) {
//     return getspikesfb(request);
//   });
//   crow_route(app, "/spikes")
//   ([this](const crow::request& request) {
//     return getspikes(request);
//   });
//   crow_route(app, "/version")
//   ([this](const crow::request& request) {
//     return getversion(request);
//   });
//   crow_route(app, "/simulationtimeinfo")
//   ([this](const crow::request& request) {
//     return getcurrentsimulationtime(request);
//   });
//   crow_route(app, "/kernelstatus")
//   ([this](const crow::request& request) {
//     return getkernelstatus(request);
//   });
//   crow_route(app, "/multimeters")
//   ([this](const crow::request& request) {
//     return getmultimeters(request);
//   });
//   crow_route(app, "/multimeter_measurement")
//   ([this](const crow::request& request) {
//     return getmultimetermeasurement(request);
//   });
//   crow_route(app, "/nodecollections")
//   ([this](const crow::request& request) {
//     return getcollections(request);
//   });
//   crow_route(app, "/nodes")
//   ([this](const crow::request& request) {
//     return getnodes(request);
//   });
//   crow_route(app, "/spikerecorders")
//   ([this](const crow::request& request) {
//     return getspikerecorders(request);
//   });
//   crow_route(app, "/")
//   ([]() {
//     return "hello, world!";
//   });
//
//   http_listener_.open().wait();
//   app.stream_threshold(std::numeric_limits<unsigned int>::max());
//   crow_server = app.port(18080 + nest::kernel().mpi_manager.get_rank()).multithreaded().run_async();
//   app.wait_for_server_start();
//
//   std::cout << "[insite] http server is listening...\n";
//
//   // server print_server;
//   //
//   //   print_server.set_message_handler(&on_message);
//   //   print_server.set_access_channels(websocketpp::log::alevel::all);
//   //   print_server.set_error_channels(websocketpp::log::elevel::all);
//   //
//   //   print_server.init_asio();
//   //   print_server.listen(9002);
//   //   print_server.start_accept();
//   //
//   //   print_server.run();
// }

void HttpServer::ClearSimulationHasEnded() {
  simulation_has_ended_ = -1;
}

void HttpServer::SimulationHasEnded(double end_time_) {
  simulation_has_ended_ = end_time_;
}

crow::response HttpServer::GetVersion(const crow::request& request) {
  rapidjson::Document document;
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.SetObject();
  document.AddMember("version", "1.2", document.GetAllocator());

  document.Accept(writer);
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetCurrentSimulationTime(
    const crow::request& request) {
  rapidjson::Document document(rapidjson::kObjectType);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  document.AddMember("current", storage_->GetCurrentSimulationTime(), document.GetAllocator());
  document.AddMember("begin", storage_->GetSimulationBeginTime(), document.GetAllocator());
  document.AddMember("end", storage_->GetSimulationEndTime(), document.GetAllocator());
  document.AddMember("stepSize", nest::Time(nest::Time::step(1)).get_ms(), document.GetAllocator());

  document.Accept(writer);
  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetKernelStatus(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  storage_->GetKernelStatus(writer);

  return crow::response(buffer.GetString());
}

// web::http::http_response HttpServer::GetVersion(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//   response.set_body(web::json::value::string("1.1"));
//
//   return response;
// }

// web::http::http_response HttpServer::GetKernelStatus(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//   response.set_body(storage_->GetKernelStatus());
//
//   return response;
// }

// web::http::http_response HttpServer::GetCurrentSimulationTime(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//
//   web::json::value response_body = web::json::value::object();
//   response_body["current"] = storage_->GetCurrentSimulationTime();
//   response_body["begin"] = storage_->GetSimulationBeginTime();
//   response_body["end"] = storage_->GetSimulationEndTime();
//   response_body["stepSize"] = nest::Time(nest::Time::step(1)).get_ms();
//   response.set_body(response_body);
//   return response;
// }

// web::http::http_response HttpServer::GetCollections(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//
//   const auto node_collections = storage_->GetNodeCollections();
//
//   web::json::value response_body =
//       web::json::value::array(node_collections.size());
//   for (size_t i = 0; i < node_collections.size(); ++i) {
//     response_body[i] = web::json::value::object();
//     response_body[i]["nodeCollectionId"] = i;
//
//     auto nodes = web::json::value::object();
//     nodes["firstId"] = node_collections[i].first_node_id;
//     nodes["lastId"] =
//         node_collections[i].first_node_id + node_collections[i].node_count - 1;
//     nodes["count"] = node_collections[i].node_count;
//     response_body[i]["nodes"] = nodes;
//
//     auto model = web::json::value::object();
//     model["name"] = web::json::value(node_collections[i].model_name);
//     model["status"] = web::json::value::parse(node_collections[i].model_status);
//
//     response_body[i]["model"] = model;
//   }
//   response.set_body(response_body);
//
//   return response;
// }

crow::response HttpServer::GetCollections(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  const auto node_collections = storage_->GetNodeCollections();

  // web::json::value response_body =
  //     web::json::value::array(node_collections.size());
  writer.StartArray();
  for (size_t i = 0; i < node_collections.size(); ++i) {
    writer.StartObject();
    writer.Key("nodeCollectionId");
    writer.Uint64(i);

    writer.Key("nodes");
    writer.StartObject();
    writer.Key("firstId");
    writer.Uint64(node_collections[i].first_node_id);
    writer.Key("lastId");
    writer.Uint64(node_collections[i].first_node_id + node_collections[i].node_count - 1);
    writer.Key("count");
    writer.Uint64(node_collections[i].node_count);
    writer.EndObject();

    writer.Key("model");
    writer.StartObject();
    writer.Key("name");
    writer.String(node_collections[i].model_name.c_str());
    writer.Key("status");
    const auto model_status = node_collections[i].model_status;
    writer.RawValue(model_status.c_str(), model_status.length(), rapidjson::kObjectType);
    writer.EndObject();

    writer.EndObject();
  }
  writer.EndArray();

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetNodes(
    const crow::request& request) {
  std::unordered_map<uint64_t, std::string> nodes = storage_->GetNodes();
  // web::json::value response_body = web::json::value::array(nodes.size());

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartArray();
  for (const auto& node : nodes) {
    writer.RawValue(node.second.c_str(), node.second.length(), rapidjson::kObjectType);
  }
  return crow::response(buffer.GetString());
}

// web::http::http_response HttpServer::GetNodes(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//
//   const auto parameters = web::uri::split_query(request.request_uri().query());
//   // const auto local_only_parameter = parameters.find("localOnly");
//   // const bool local_only = local_only_parameter == parameters.end()
//   //                             ? false
//   //                             : (local_only_parameter->second == "false" ||
//   //                                local_only_parameter->second == "0");
//
//   std::unordered_map<uint64_t, std::string> nodes = storage_->GetNodes();
//   web::json::value response_body = web::json::value::array(nodes.size());
//
//   unsigned int current_node_index = 0;
//   for (const auto& node : nodes) {
//     response_body[current_node_index] = web::json::value::parse(node.second);
//     ++current_node_index;
//   }
//
//   response.set_body(response_body);
//   return response;
// }

crow::response HttpServer::GetSpikeRecorders(
    const crow::request& request) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  const auto spike_detectors = storage_->GetSpikeDetectors();

  // web::json::value response_body = web::json::value::array();

  writer.StartArray();
  for (const auto& spikedetector_id_storage : spike_detectors) {
    std::vector<std::uint64_t> connected_node_ids;
    writer.StartObject();
    writer.Key("spikerecorderId");
    writer.Uint64(spikedetector_id_storage.first);

    spikedetector_id_storage.second->ExtractConnectedNodeIds(
        &connected_node_ids);

    writer.Key("nodeIds");
    writer.StartArray();
    for (size_t i = 0; i < connected_node_ids.size(); ++i) {
      writer.Uint64(connected_node_ids[i]);
    }
    writer.EndArray();
    writer.EndObject();
  }
  writer.EndArray();

  return crow::response(buffer.GetString());
}

// web::http::http_response HttpServer::GetSpikeRecorders(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//
//   const auto spike_detectors = storage_->GetSpikeDetectors();
//
//   web::json::value response_body = web::json::value::array();
//   std::vector<std::uint64_t> connected_node_ids;
//
//   for (const auto& spikedetector_id_storage : spike_detectors) {
//     web::json::value spikedetector_data = web::json::value::object();
//     spikedetector_data["spikerecorderId"] = spikedetector_id_storage.first;
//
//     spikedetector_id_storage.second->ExtractConnectedNodeIds(
//         &connected_node_ids);
//     spikedetector_data["nodeIds"] =
//         web::json::value::array(connected_node_ids.size());
//     for (size_t i = 0; i < connected_node_ids.size(); ++i) {
//       spikedetector_data["nodeIds"][i] = connected_node_ids[i];
//     }
//
//     response_body[response_body.size()] = spikedetector_data;
//   }
//
//   response.set_body(response_body);
//
//   return response;
// }
//
// web::http::http_response HttpServer::GetSpikeDetectors(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//
//   const auto spike_detectors = storage_->GetSpikeDetectors();
//
//   web::json::value response_body = web::json::value::array();
//   std::vector<std::uint64_t> connected_node_ids;
//
//   for (const auto& spikedetector_id_storage : spike_detectors) {
//     web::json::value spikedetector_data = web::json::value::object();
//     spikedetector_data["spikedetectorId"] = spikedetector_id_storage.first;
//
//     spikedetector_id_storage.second->ExtractConnectedNodeIds(
//         &connected_node_ids);
//     spikedetector_data["nodeIds"] =
//         web::json::value::array(connected_node_ids.size());
//     for (size_t i = 0; i < connected_node_ids.size(); ++i) {
//       spikedetector_data["nodeIds"][i] = connected_node_ids[i];
//     }
//
//     response_body[response_body.size()] = spikedetector_data;
//   }
//
//   response.set_body(response_body);
//
//   return response;
// }

crow::response HttpServer::GetSpikesFB(const crow::request& request) {
  spdlog::stopwatch spikes_fb;
  // const auto parameters = web::uri::split_query(request.request_uri().query());
  std::unordered_map<std::string, std::string> parameters;

  // web::http::http_response response(web::http::status_codes::OK);

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
      // return CreateErrorResponse(web::http::status_codes::BadRequest,
      //                            {"InvalidNodeCollectionID"});
    }
  }

  const auto spike_detector_id_parameter = parameters.find("spikedetectorId");

  std::vector<Spike> spikes;

  if (spike_detector_id_parameter == parameters.end()) {
    std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id << ")]" << std::endl;
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes(&spikes, from_time, to_time, from_node_id, to_node_id, &filter_node_ids);
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
      // return CreateErrorResponse(web::http::status_codes::BadRequest,
      // {"InvalidSpikeDetectorId"});
    } else {
      spike_detector->ExtractSpikes(&spikes, from_time, to_time, from_node_id,
                                    to_node_id, &filter_node_ids);
    }
  }

  bool last_frame = simulation_has_ended_ != -1 && (to_time >= simulation_has_ended_);
  spdlog::stopwatch fb_building;
  flatbuffers::FlatBufferBuilder fbb;
  auto fb_spikes = fbb.CreateVectorOfNativeStructs<Test::Foo::Spikes>(spikes, flatbuffers::Pack);
  auto fb_spike_table = Test::Foo::CreateSpikyTable(fbb, fb_spikes);
  spdlog::info("Fb building: {}", fb_building.elapsed());
  spdlog::info("Get Spikes: {}", spikes_fb.elapsed());
  fbb.Finish(fb_spike_table);
  return crow::response(std::string((const char*)fbb.GetBufferPointer(), fbb.GetSize()));
}

crow::response HttpServer::GetSpikes(const crow::request& request) {
  spdlog::stopwatch sw;
  // const auto parameters = web::uri::split_query(request.request_uri().query());
  std::unordered_map<std::string, std::string> parameters;

  // web::http::http_response response(web::http::status_codes::OK);

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
  spdlog::info("Parameter parsing took: {}", sw.elapsed());
  spdlog::info("Parameters: from_time: {}, to_time: {}", from_time, to_time);
  spdlog::info("Parameter string: {}", request.raw_url);
  sw.reset();
  std::uint64_t from_node_id = 0;
  std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
  if (!filter_node_ids.empty()) {
    from_node_id = *std::min_element(filter_node_ids.begin(), filter_node_ids.end());
    to_node_id = *std::max_element(filter_node_ids.begin(), filter_node_ids.end());
  }

  spdlog::info("Min/Max nodeId took: {}", sw.elapsed());
  sw.reset();
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
      // return CreateErrorResponse(web::http::status_codes::BadRequest,
      //                            {"InvalidNodeCollectionID"});
    }
  }

  const auto spike_detector_id_parameter = parameters.find("spikedetectorId");

  std::vector<Spike> spikes;

  if (spike_detector_id_parameter == parameters.end()) {
    std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id << ")]" << std::endl;
    std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
    for (const auto& spike_detector_id_storage : spike_detectors) {
      spike_detector_id_storage.second->ExtractSpikes(&spikes, from_time, to_time, from_node_id, to_node_id, &filter_node_ids);
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
      // return CreateErrorResponse(web::http::status_codes::BadRequest,
      // {"InvalidSpikeDetectorId"});
    } else {
      spike_detector->ExtractSpikes(&spikes, from_time, to_time, from_node_id,
                                    to_node_id, &filter_node_ids);
    }
  }

  spdlog::info("Extracting spikes took: {}", sw.elapsed());
  sw.reset();
  bool last_frame = simulation_has_ended_ != -1 && (to_time >= simulation_has_ended_);
  spdlog::info("sim_has_ended: {}, to_time: {}, lastFrame: {}",simulation_has_ended_, to_time, last_frame);
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("simulationTimes");
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Double(spike.simulation_time);
  }
  writer.EndArray();

  writer.Key("nodeIds");
  writer.StartArray();
  for (const auto& spike : spikes) {
    writer.Int(spike.node_id);
  }
  writer.EndArray();
  writer.Key("lastFrame");
  writer.Bool(last_frame);
  writer.EndObject();

  spdlog::info("Serializing spikes took: {}", sw.elapsed());
  return crow::response(s.GetString());
}
// web::http::http_response HttpServer::GetSpikes(
//     const web::http::http_request& request) {
//   const auto parameters = web::uri::split_query(request.request_uri().query());
//
//   web::http::http_response response(web::http::status_codes::OK);
//
//   const auto from_time_parameter = parameters.find("fromTime");
//   const double from_time = from_time_parameter == parameters.end()
//                                ? 0.0
//                                : std::stod(from_time_parameter->second);
//
//   const auto to_time_parameter = parameters.find("toTime");
//   const double to_time = to_time_parameter == parameters.end()
//                              ? std::numeric_limits<double>::infinity()
//                              : std::stod(to_time_parameter->second);
//
//   const auto node_collection_parameter = parameters.find("nodeCollectionId");
//
//   const auto parameter_gids = parameters.find("nodeIds");
//   auto parseString = (parameter_gids == parameters.end()) ? std::string("") : parameter_gids->second;
//   auto filter_node_ids = CommaListToUintVector(parseString);
//
//   std::uint64_t from_node_id = 0;
//   std::uint64_t to_node_id = std::numeric_limits<std::uint64_t>::max();
//   if (!filter_node_ids.empty()) {
//     from_node_id = *std::min_element(filter_node_ids.begin(), filter_node_ids.end());
//     to_node_id = *std::max_element(filter_node_ids.begin(), filter_node_ids.end());
//   }
//   if (node_collection_parameter != parameters.end()) {
//     const std::uint64_t node_collection_id =
//         std::stoull(node_collection_parameter->second);
//     if (node_collection_id < storage_->GetNodeCollectionCount()) {
//       const NodeCollection node_collection =
//           storage_->GetNodeCollection(node_collection_id);
//       from_node_id = node_collection.first_node_id;
//       to_node_id = node_collection.first_node_id + node_collection.node_count - 1;
//     } else {
//       return CreateErrorResponse(web::http::status_codes::BadRequest,
//                                  {"InvalidNodeCollectionID"});
//     }
//   }
//
//   const auto spike_detector_id_parameter = parameters.find("spikedetectorId");
//
//   std::vector<Spike> spikes;
//
//   if (spike_detector_id_parameter == parameters.end()) {
//     std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id << ")]" << std::endl;
//     std::unordered_map<std::uint64_t, std::shared_ptr<SpikedetectorStorage>> spike_detectors = storage_->GetSpikeDetectors();
//     for (const auto& spike_detector_id_storage : spike_detectors) {
//       spike_detector_id_storage.second->ExtractSpikes(&spikes, from_time, to_time, from_node_id, to_node_id, &filter_node_ids);
//     }
//
//   } else {
//     const auto spike_detector_id =
//         std::stoll(spike_detector_id_parameter->second);
//     const auto spike_detector =
//         storage_->GetSpikeDetectorStorage(spike_detector_id);
//
//     std::cout << "[insite] Querying spikes: [time: (" << from_time << " - " << to_time << ")] [nodes: (" << from_node_id << " - " << to_node_id
//               << ")], spikedetector=" << spike_detector_id << std::endl;
//
//     if (spike_detector == nullptr) {
//       return CreateErrorResponse(web::http::status_codes::BadRequest,
//                                  {"InvalidSpikeDetectorId"});
//     } else {
//       spike_detector->ExtractSpikes(&spikes, from_time, to_time, from_node_id,
//                                     to_node_id, &filter_node_ids);
//     }
//   }
//
//   bool last_frame = simulation_has_ended_ != -1 && (to_time >= simulation_has_ended_);
//
//   rapidjson::StringBuffer s;
//   rapidjson::Writer<rapidjson::StringBuffer> writer(s);
//
//   writer.StartObject();
//   writer.Key("simulationTimes");
//   writer.StartArray();
//   for (const auto& spike : spikes) {
//     writer.Double(spike.simulation_time);
//   }
//   writer.EndArray();
//
//   writer.Key("nodeIds");
//   writer.StartArray();
//   for (const auto& spike : spikes) {
//     writer.Int(spike.node_id);
//   }
//   writer.EndArray();
//   writer.Key("lastFrame");
//   writer.Bool(last_frame);
//   writer.EndObject();
//
//   web::http::http_response res(web::http::status_codes::OK);
//   res.set_body(s.GetString());
//   return res;
// }
//
// web::http::http_response HttpServer::GetMultimeters(
//     const web::http::http_request& request) {
//   web::http::http_response response(web::http::status_codes::OK);
//
//   const auto multimeters = storage_->GetMultimeters();
//
//   rapidjson::StringBuffer s;
//   rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
//
//   writer.StartArray();
//
//   for (const auto& multimeter_id_storage : multimeters) {
//     multimeter_id_storage.second->Serialize(writer);
//   }
//   writer.EndArray();
//
//   response.set_body(s.GetString());
//   return response;
// }

crow::response HttpServer::GetMultimeters(
    const crow::request& request) {
  const auto multimeters = storage_->GetMultimeters();

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

  writer.StartArray();

  for (const auto& multimeter_id_storage : multimeters) {
    multimeter_id_storage.second->Serialize(writer);
  }
  writer.EndArray();

  return crow::response(buffer.GetString());
}

crow::response HttpServer::GetMultimeterMeasurement(
    const crow::request& request) {
  // const auto from_time_parameter = parameters.find("fromTime");
  const double from_time = GetParamValueOrDefault(request.url_params, "fromTime", 0.0);

  const double to_time = GetParamValueOrDefault(request.url_params, "toTime", std::numeric_limits<double>::infinity());

  // std::cout << "Query: " << request.raw_url << std::endl;
  auto parseString = GetParamValueOrDefault(request.url_params, "nodeIds", std::string(""));
  auto filter_node_ids = CommaListToUintVector(parseString);

  std::copy(filter_node_ids.begin(), filter_node_ids.end(), std::ostream_iterator<std::uint64_t>(std::cout, ","));
  spdlog::debug("filter nodes: {}", filter_node_ids);
  std::cout << std::endl;

  const auto attribute_name = request.url_params.get("attribute");
  if (attribute_name == nullptr) {
    // return CreateErrorResponse(web::http::status_codes::BadRequest, {"MissingRequiredParameter", "The 'attributeName' parameter is missing from the request."});
    return crow::response("The 'attributeName' parameter is missing from the request.");
  }

  const auto multimeter_id_parameter = request.url_params.get("multimeterId");

  if (multimeter_id_parameter == nullptr) {
    // return CreateErrorResponse(web::http::status_codes::BadRequest, {"MissingRequiredParameter", "The 'multimeterId' parameter is missing from the request."});
    return crow::response("The 'multimeterId' parameter is missing from the request.");
  }
  const uint64_t multimeter_id = std::stoull(multimeter_id_parameter);

  const auto multimeters = storage_->GetMultimeters();
  const auto multimeter = multimeters.find(multimeter_id);
  if (multimeter == multimeters.end()) {
    return crow::response("InvalidMultimeterId");
  }

  rapidjson::StringBuffer s;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

  spdlog::info("Getting MM Measurements {} for {} from {} to {}", attribute_name, multimeter->second->id_, from_time, to_time);
  multimeter->second->ExtractMeasurements(writer, attribute_name, filter_node_ids, from_time, to_time);
  return crow::response(s.GetString());
}

// web::http::http_response HttpServer::GetMultimeterMeasurement(
//     const web::http::http_request& request) {
//   const auto parameters = web::uri::split_query(request.request_uri().query());
//
//   const auto from_time_parameter = parameters.find("fromTime");
//   const double from_time = from_time_parameter == parameters.end() ? 0.0 : std::stod(from_time_parameter->second);
//
//   const auto to_time_parameter = parameters.find("toTime");
//   const double to_time = to_time_parameter == parameters.end() ? std::numeric_limits<double>::infinity() : std::stod(to_time_parameter->second);
//
//   std::cout << "Query: " << request.request_uri().query() << std::endl;
//   const auto node_ids_parameter = parameters.find("nodeIds");
//   auto parseString = (node_ids_parameter == parameters.end()) ? std::string("") : node_ids_parameter->second;
//   auto filter_node_ids = CommaListToUintVector(parseString);
//   std::copy(filter_node_ids.begin(), filter_node_ids.end(), std::ostream_iterator<std::uint64_t>(std::cout, ","));
//   std::cout << std::endl;
//
//   const auto attribute_name_parameter = parameters.find("attribute");
//   if (attribute_name_parameter == parameters.end()) {
//     return CreateErrorResponse(web::http::status_codes::BadRequest, {"MissingRequiredParameter", "The 'attributeName' parameter is missing from the request."});
//   }
//   const std::string attribute_name = attribute_name_parameter->second;
//
//   const auto multimeter_id_parameter = parameters.find("multimeterId");
//   if (multimeter_id_parameter == parameters.end()) {
//     return CreateErrorResponse(web::http::status_codes::BadRequest, {"MissingRequiredParameter", "The 'multimeterId' parameter is missing from the request."});
//   }
//   const uint64_t multimeter_id = std::stoull(multimeter_id_parameter->second);
//
//   const auto multimeters = storage_->GetMultimeters();
//   const auto multimeter = multimeters.find(multimeter_id);
//   if (multimeter == multimeters.end()) {
//     return CreateErrorResponse(web::http::status_codes::BadRequest, {"InvalidMultimeterId"});
//   }
//
//   web::http::http_response response(web::http::status_codes::OK);
//   rapidjson::StringBuffer s;
//   rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
//
//   multimeter->second->ExtractMeasurements(writer, attribute_name, filter_node_ids, from_time, to_time);
//   response.set_body(s.GetString());
//   return response;
// }
//
// web::json::value HttpServer::Error::Serialize() const {
//   web::json::value error = web::json::value::object();
//   error["code"] = web::json::value(code);
//   error["message"] = web::json::value(message);
//   return error;
// }
//
// web::http::http_response HttpServer::CreateErrorResponse(
//
//     web::http::status_code status_code,
//     const Error& error) {
//   web::http::http_response response(status_code);
//
//   web::json::value body = web::json::value::object();
//   body["error"] = error.Serialize();
//   response.set_body(body);
//
//   return response;
// }

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
