#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Includes from libnestutil:
#include "compose.hpp"

// Includes from nestkernel:
#include "connection_id.h"
#include "connection_label.h"
#include "dict.h"
#include "dictdatum.h"
#include "extern/flatbuffers/tests/native_type_test_impl.h"
#include "extern/spdlog/include/spdlog/spdlog.h"
#include "flatbuffers/flatbuffer_builder.h"
#include "insite_nest_events.h"
#include "kernel_manager.h"
#include "nest_datums.h"
#include "node_collection.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "recording_device.h"
#include "schema_generated.h"
#include "simulation_manager.h"
#include "spatial.h"
#include "spdlog/spdlog.h"
// #include "topology.h"

// Includes from sli:
#include "dictutils.h"
#include "recording_backend_insite.h"
#include "serialize.hpp"
#include "websocketpp/close.hpp"
#include "websocketpp/frame.hpp"

namespace flatbuffers {

static Test::Foo::Spikes Pack(const insite::Spike& spike) {
  return Test::Foo::Spikes{spike.simulation_time, spike.node_id};
};
}  // namespace flatbuffers
namespace insite {

void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
  std::cout << msg->get_payload() << std::endl;
}
typedef websocketpp::client<websocketpp::config::asio_client> client;

RecordingBackendInsite::RecordingBackendInsite()
    : http_server_("http://0.0.0.0:" + get_port_string(), &data_storage_) {
  std::string uri = "ws://localhost:9011/nest";
  c.set_access_channels(websocketpp::log::alevel::all);
  c.clear_access_channels(websocketpp::log::alevel::frame_payload);
  c.set_error_channels(websocketpp::log::elevel::all);

  // Initialize ASIO
  c.init_asio();

  // Register our message handler
  c.set_message_handler(&on_message);

  websocketpp::lib::error_code ec;
  con = c.get_connection(uri, ec);
  if (ec) {
    std::cout << "could not create connection because: " << ec.message() << std::endl;
  }

  // Note that connect here only requests a connection. No network messages are
  // exchanged until the event loop starts running in the next line.
  c.connect(con);
  // Start the ASIO io_service run loop
  // this will cause a single connection to be made to the server. c.run()
  // will exit when this connection is closed.
  ws_thread = std::thread([this]() { c.run(); });
}

RecordingBackendInsite::~RecordingBackendInsite() throw() {
  try {
    con->close(websocketpp::close::status::normal, "Simulation finished");
  } catch (std::exception e) {
    spdlog::error("{}", e.what());
  }
  c.stop();
  ws_thread.join();
}

void RecordingBackendInsite::initialize() {
  std::cout << "[insite] initialize" << std::endl;
  data_storage_.Reset();
  http_server_.ClearSimulationHasEnded();
}

void RecordingBackendInsite::finalize() {
}

void RecordingBackendInsite::enroll(const nest::RecordingDevice& device,
                                    const DictionaryDatum& params) {
  updateValue<bool>(params, "send_positions", send_positions);
  spdlog::info("ENROLL: {}", send_positions);
  if (device.get_type() == nest::RecordingDevice::SPIKE_RECORDER) {
    std::cout << "[insite] enroll spike recorder (" << device.get_label() << ") with id " << device.get_node_id() << "\n";
    data_storage_.CreateSpikeDetectorStorage(device.get_node_id());
  } else if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    std::cout << "[insite] enroll multimeter (" << device.get_label() << ") with id " << device.get_node_id() << "\n";
    data_storage_.CreateMultimeterStorage(device.get_node_id());
  }
}

void RecordingBackendInsite::disenroll(const nest::RecordingDevice& device) {
}

void RecordingBackendInsite::set_value_names(
    const nest::RecordingDevice& device,
    const std::vector<Name>& double_value_names,
    const std::vector<Name>& long_value_names) {
  if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    data_storage_.GetMultimeterStorage(device.get_node_id())->SetAttributes(double_value_names, long_value_names);
  }
}

void RecordingBackendInsite::prepare() {
  std::unordered_map<uint64_t, int> nc_maps;
  int nc_counter = 0;
  std::cout << "[insite] prepare" << std::endl;
  DictionaryDatum properties(new Dictionary());
  nest::NodeCollectionPTR local_nodes = nest::kernel().node_manager.get_nodes(properties, true);
  data_storage_.SetNodesFromCollection(local_nodes);

  for (const auto& spike_detector : data_storage_.GetSpikeDetectors()) {
    spike_detector.second->Prepare(local_nodes);
  }
  for (const auto& multimeter : data_storage_.GetMultimeters()) {
    multimeter.second->Prepare(local_nodes);
  }

  UpdateKernelStatus();
  rapidjson::StringBuffer Buf;
  rapidjson::Writer<rapidjson::StringBuffer> Writer(Buf);
  rapidjson::StringBuffer PrepareEventJSON = InsiteSimulatorEvent(SimulationEvents::Cleanup).SerializeToJSON();
  con->send(PrepareEventJSON.GetString());
  Buf.Clear();
  Writer.StartObject();
  Writer.Key("type");
  Writer.Int(2);
  Writer.Key("nodes");
  Writer.StartArray();
  for (const nest::NodeIDTriple& node_id_triple : *local_nodes.get()) {
    auto node_id = node_id_triple.node_id;
    nest::Node* nest_node = nest::kernel().node_manager.get_node_or_proxy(node_id_triple.node_id);
    int nc_first_node = -1;
    if (nest_node) {
      nest::NodeCollectionPTR node_collection = nest_node->get_nc();
      auto nc_first_node_ptr = node_collection->get_metadata();
      if (nc_first_node_ptr) {
        nc_first_node = nc_first_node_ptr->get_first_node_id();
      }
    }
    Writer.StartObject();
    Writer.Key("node_id");
    Writer.Uint64(node_id);
    Writer.Key("position");
    std::vector<double> position = nest::get_position(node_id_triple.node_id);
    if (!std::isnan(position[0])) {
      Writer.StartArray();
      for (const auto& pos : position) {
        Writer.Double(pos);
      }
      Writer.EndArray();
    } else {
      Writer.Null();
    }
    Writer.Key("Population");
    int nc_id = -1;
    if (nc_maps.count(nc_first_node)) {
      nc_id = nc_maps[nc_first_node];
    } else {
      nc_maps[nc_first_node] = nc_counter++;
      nc_id = nc_counter;
    }
    Writer.Int(nc_id);

    Writer.Key("Model");
    nest::Model* model = nest::kernel().model_manager.get_node_model(node_id_triple.model_id);
    Writer.String(model->get_name().c_str());
    Writer.EndObject();
  }
  Writer.EndArray();
  Writer.EndObject();
  con->send(Buf.GetString());
  std::deque<nest::ConnectionID> conn_deq;

  if (send_positions) {
    Buf.Clear();
    Writer.StartObject();
    Writer.Key("type");
    Writer.Int(3);
    Writer.Key("connections");
    Writer.StartArray();
    spdlog::info("Num connections: {}", nest::kernel().connection_manager.get_num_connections());
    for (int i = 0; i < nest::kernel().model_manager.get_num_connection_models(); i++) {
      nest::kernel().connection_manager.get_connections(conn_deq, nest::NodeCollectionPTR(0), nest::NodeCollectionPTR(0), i, -1);
    }
    for (auto& conn : conn_deq) {
      spdlog::info("conn: {}, {}", conn.get_source_node_id(), conn.get_target_node_id());
    }

    for (auto& res : conn_deq) {
      auto source = res.get_source_node_id();
      auto target = res.get_target_node_id();

      Writer.StartArray();
      Writer.Int(source);
      Writer.Int(target);
      Writer.EndArray();
    }
    Writer.EndArray();
    Writer.EndObject();
    con->send(Buf.GetString());
  }
}

void RecordingBackendInsite::cleanup() {
  std::cout << "[Insite] Cleanup" << std::endl;

  rapidjson::StringBuffer CleanupEventJSON = InsiteSimulatorEvent(SimulationEvents::Cleanup).SerializeToJSON();
  con->send(CleanupEventJSON.GetString());
}

void RecordingBackendInsite::pre_run_hook() {
  std::cout << "[Insite] Pre Run Hook" << std::endl;
  data_storage_.SetSimulationTimeRange(
      nest::kernel().simulation_manager.run_start_time().get_ms(),
      nest::kernel().simulation_manager.run_end_time().get_ms());
  spdlog::info("Run duration: {}", nest::kernel().simulation_manager.run_duration().get_ms());

  UpdateKernelStatus();
  rapidjson::StringBuffer PreRunEventJSON = InsiteSimulatorEvent(SimulationEvents::PreRunHook).SerializeToJSON();
  con->send(PreRunEventJSON.GetString());
}

void RecordingBackendInsite::post_run_hook() {
  std::cout << "[Insite] Post Run Hook" << std::endl;
  data_storage_.SetCurrentSimulationTime(data_storage_.GetSimulationEndTime());
  http_server_.SimulationHasEnded(data_storage_.GetSimulationEndTime());
  UpdateKernelStatus();

  rapidjson::StringBuffer PostRunEventJSON = InsiteSimulatorEvent(SimulationEvents::PostRunHook).SerializeToJSON();
  con->send(PostRunEventJSON.GetString());
}

void RecordingBackendInsite::post_step_hook() {
  auto current_time = nest::kernel().simulation_manager.get_clock().get_ms();
  data_storage_.SetCurrentSimulationTime(current_time);

  flatbuffers::FlatBufferBuilder fbb;
  auto fb_spikes = fbb.CreateVectorOfNativeStructs<Test::Foo::Spikes>(spikes, flatbuffers::Pack);
  auto fb_spike_table = Test::Foo::CreateSpikyTable(fbb, fb_spikes);
  fbb.Finish(fb_spike_table);
  con->send(fbb.GetBufferPointer(), fbb.GetSize(), websocketpp::frame::opcode::BINARY);
  spikes.clear();
}

void RecordingBackendInsite::write(const nest::RecordingDevice& device,
                                   const nest::Event& event,
                                   const std::vector<double>& double_values,
                                   const std::vector<long>& long_values) {
  const auto sender_gid = event.get_sender_node_id();
  const auto time_stamp = event.get_stamp().get_ms();
  if (device.get_type() == nest::RecordingDevice::SPIKE_RECORDER) {
    data_storage_.AddSpike(device.get_node_id(), time_stamp, sender_gid);
    spikes.push_back({time_stamp, sender_gid});
  } else if (device.get_type() == nest::RecordingDevice::MULTIMETER) {
    data_storage_.AddMultimeterMeasurement(device.get_node_id(), time_stamp, sender_gid, double_values, long_values);
  }
  latest_simulation_time_ = std::max(latest_simulation_time_, time_stamp);
}

void RecordingBackendInsite::set_status(const DictionaryDatum& params) {
}

void RecordingBackendInsite::get_status(DictionaryDatum& params) const {
}

void RecordingBackendInsite::check_device_status(const DictionaryDatum& params) const {
}

void RecordingBackendInsite::get_device_defaults(DictionaryDatum& params) const {
}

void RecordingBackendInsite::get_device_status(const nest::RecordingDevice& device, DictionaryDatum& params) const {
}

std::string RecordingBackendInsite::get_port_string() const {
  // Ports can be configured via docker
  return std::to_string(9000 + nest::kernel().mpi_manager.get_rank());
}

void RecordingBackendInsite::UpdateKernelStatus() {
  DictionaryDatum kernel_status(new Dictionary());
  nest::kernel().get_status(kernel_status);
  data_storage_.SetDictKernelStatus(kernel_status);
}

}  // namespace insite
