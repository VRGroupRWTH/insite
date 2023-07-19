#ifndef RECORDING_BACKEND_INSITE_H
#define RECORDING_BACKEND_INSITE_H

#include <unordered_map>
// #include <cpprest/http_client.h>

#include "http_server.hpp"
#include "nest_types.h"
#include "node_collection.h"
#include "recording_backend.h"
#include "storage/data_storage.hpp"

#include "neuron_info.hpp"

#undef UNKNOWN
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include "websocketpp/connection.hpp"

namespace insite {

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
typedef websocketpp::client<websocketpp::config::asio_client> client;

enum SimulationEvents {
  Prepare = 0,
  PreRunHook = 1,
  PostRunHook = 2,
  Cleanup = 3
};

class RecordingBackendInsite : public nest::RecordingBackend {
 public:
  RecordingBackendInsite();

  ~RecordingBackendInsite() throw() override;

  void initialize() override;
  void finalize() override;

  void enroll(const nest::RecordingDevice& device,
              const DictionaryDatum& params) override;

  void disenroll(const nest::RecordingDevice& device) override;

  void set_value_names(const nest::RecordingDevice& device,
                       const std::vector<Name>& double_value_names,
                       const std::vector<Name>& long_value_names) override;

  void prepare() override;

  void cleanup() override;

  void pre_run_hook() override;

  void post_run_hook() override;

  void post_step_hook() override;

  void write(const nest::RecordingDevice& device, const nest::Event& event, const std::vector<double>& double_values, const std::vector<long>& long_values) override;

  void set_status(const DictionaryDatum& params) override;

  void get_status(DictionaryDatum& params) const override;

  void check_device_status(const DictionaryDatum& params) const override;

  void get_device_defaults(DictionaryDatum& params) const override;

  void get_device_status(const nest::RecordingDevice& device,
                         DictionaryDatum& params) const override;

 private:
  client c;
  client::connection_ptr con;
  std::thread ws_thread;
  std::string get_port_string() const;
  void UpdateKernelStatus();
  DataStorage data_storage_;
  HttpServer http_server_;
  // int simulation_node_id_;
  double latest_simulation_time_ = 0.0;
  std::vector<Spike> spikes;
  bool send_positions = false;
  // bool prepare_finished = false;
};

}  // namespace insite

#endif  // RECORDING_BACKEND_INSITE_H
