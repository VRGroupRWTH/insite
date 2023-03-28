#pragma once
#include "opcodes.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "websocket_server.h"
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <spdlog/spdlog.h>
#include <thread>
#include <tvb/monitor.h>
#include <unistd.h>

namespace insite {
class TvbHandler {
public:
  TvbHandler() = default;

  void ParseSimInfoReturn(const std::string &payload);
  void ParseDataPacket(const std::string &payload);
  void ParseNewMonitorPacket(const std::string &payload);
  void ParseSimInfo(const std::string &payload);
  void
  SerializeMonitorsJson(rapidjson::Writer<rapidjson::StringBuffer> &writer);
  void AddMessageIntoQueue(std::string &&msg);

  void StartThread() {
    consumer_thread_ = std::thread(&TvbHandler::Consumer, this);
  }

  void StopThread() {
    runConsumerLoop_ = false;
    consumer_thread_.join();
  }

  void Consumer();

  WebsocketServer *srv;
  // WebsocketServer *srv;
  uint64_t bytes = 0;
  // private:

  std::promise<std::string> sim_info_promise;
  bool runConsumerLoop_ = true;
  std::vector<std::string> message_queue_;
  uint32_t number_of_monitors = 0;
  std::thread consumer_thread_;
  std::vector<TvbMonitor<double>> double_monitors_;
  std::condition_variable var_;
  std::mutex mut_;

  rapidjson::Document sim_info;
};
} // namespace insite
