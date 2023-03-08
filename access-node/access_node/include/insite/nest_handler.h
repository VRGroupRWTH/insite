#pragma once
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <thread>
#include <websocketpp/frame.hpp>
#include "opcodes.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "websocket_server.h"
// #include "tvbMonitor.h"

namespace insite {
class NestHandler {
 public:
  NestHandler() = default;
  void ParseDataPacket(const std::string& payload);
  void SerializeMonitorsJson(
      rapidjson::Writer<rapidjson::StringBuffer>& writer);
  void AddMessageIntoQueue(websocketpp::frame::opcode::value value,
                           std::string&& msg);

  void StartThread() {
    consumer_thread_ = std::thread(&NestHandler::Consumer, this);
  }

  void StopThread() {
    runConsumerLoop_ = false;
    consumer_thread_.join();
  }

  void Consumer();

  // SpikeStorage storage;
  WebsocketServer* ws_server;
  uint64_t bytes = 0;
  // private:
  bool runConsumerLoop_ = true;
  // std::vector<std::pair<websocketpp::frame::opcode::value ,std::string>>
  // message_queue_;
  std::deque<std::pair<websocketpp::frame::opcode::value, std::string>>
      message_queue_;
  uint32_t number_of_monitors = 0;
  std::thread consumer_thread_;
  // std::vector<TvbMonitor<double>> double_monitors_;
  std::condition_variable var_;
  std::mutex mut_;
};
}  // namespace insite
