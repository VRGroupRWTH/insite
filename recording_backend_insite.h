#ifndef RECORDING_BACKEND_INSITE_H
#define RECORDING_BACKEND_INSITE_H

#include <unordered_map>
#include <cpprest/http_client.h>

#include "data_storage.hpp"
#include "http_server.hpp"
#include "recording_backend.h"
#include "nest_types.h"
#include "node_collection.h"

namespace insite {

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

  void write(const nest::RecordingDevice& device, const nest::Event& event,
             const std::vector<double>& double_values,
             const std::vector<long>& long_values) override;

  void set_status(const DictionaryDatum& params) override;

  void get_status(DictionaryDatum& params) const override;

  void check_device_status(const DictionaryDatum& params) const override;

  void get_device_defaults(DictionaryDatum& params) const override;

  void get_device_status(const nest::RecordingDevice& device,
                         DictionaryDatum& params) const override;

 private:
  std::string get_port_string() const;

  DataStorage data_storage_;
  HttpServer http_server_;
  web::http::client::http_client info_node_;
  std::string address_;
  std::vector<nest::index> gids_;
  std::vector<nest::index> new_gids_;
  std::unordered_map<nest::NodeCollectionPTR, int64_t> registered_node_collections_;  
  std::vector<nest::NodeCollectionPTR> node_collections_to_register_;
  nest::delay latest_simulation_time_ = 0;
};

}  // namespace insite

#endif  // RECORDING_BACKEND_INSITE_H
