#pragma once
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <condition_variable>
#include <cstdint>
#include <map>
#include <mutex>
#include <thread>
#include "opcodes.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "tvbMonitor.h"
#include "websocketpp/common/connection_hdl.hpp"
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/roles/server_endpoint.hpp"
#include "resource_flags.h"

namespace insite {
using Server = websocketpp::server<websocketpp::config::asio>;

enum PeerType { kUnknown = 0, kClient, kServer };

struct ClientParams {
  std::uint32_t id;
  PeerType type;
  ResourceFlag resource_flags;
};

class WebsocketSender {
 public:
  WebsocketSender(std::map<websocketpp::connection_hdl, ClientParams, std::owner_less<websocketpp::connection_hdl>>& connection_map, Server& websocket_server, std::mutex& mutex, std::condition_variable& cond)
      : connection_map(connection_map),var_(cond), mut_(mutex), websocket_server(websocket_server) {}

  void StartThread(std::vector<WebsocketMessage>* queue) {
    consumer_thread_ = std::thread(&WebsocketSender::Consumer, this, queue);
  }

  void Consumer(std::vector<WebsocketMessage>* queue);

  // void BroadcastAll(const std::string& msg);
  void BroadcastAll(const std::string& msg, ResourceFlag resource);
  
  // private:
  std::map<websocketpp::connection_hdl, ClientParams, std::owner_less<websocketpp::connection_hdl>>& connection_map; 
  std::thread consumer_thread_;
  std::condition_variable& var_;
  std::mutex& mut_;
  Server& websocket_server;
};
}  // namespace insite
