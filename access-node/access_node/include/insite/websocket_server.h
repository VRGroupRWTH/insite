#pragma once
#include "resource_flags.h"
#include "spdlog/spdlog.h"
#include "websocket_sender.h"
#include "websocketpp/close.hpp"
#include "websocketpp/common/connection_hdl.hpp"
#include <chrono>
#include <cstdint>
#include <thread>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
using namespace std::chrono_literals;

namespace insite {
using Server = websocketpp::server<websocketpp::config::asio>;
class NestHandler;

template <typename Key, typename Value, std::size_t Size>
struct Map {
  std::array<std::pair<Key, Value>, Size> data;
  constexpr Value At(const Key& key) const {
    const auto itr =
        std::find_if(begin(data), end(data),
                     [&key](const auto& value) { return value.first == key; });
    if (itr != end(data)) {
      return itr->second;
    }
    throw std::range_error("Not Found");
  }
};

class WebsocketServer {
 public:
  WebsocketServer(std::uint64_t port);
  ~WebsocketServer();
  void Send(WebsocketMessage&& message);
  void StartServer();
  void StopServer();
  void SetTvbHandler(TvbHandler* tvb_handler);
  void SetNestHandler(NestHandler* nest_handler);

 private:
  ResourceFlag GetResourceFlag(const std::string& resource);
  void SendLoop();
  void BroadcastAll(const std::string& msg, ResourceFlag resource_flags);
  void OnMessage(const websocketpp::connection_hdl& hdl,
                 const Server::message_ptr& message);
  void OnMessageFromNESTServer(const websocketpp::connection_hdl& hdl,
                               const Server::message_ptr& message);
  void OnOpen(const websocketpp::connection_hdl& hdl);
  void OnClose(const websocketpp::connection_hdl& hdl);

  Server server_;
  static constexpr std::uint16_t kMaxMessageSizeMb = 200;
  static constexpr std::uint64_t kMaxMessageSize =
      kMaxMessageSizeMb * 1024 * 1024;
  std::uint64_t port_;
  std::uint16_t no_connections_ = 0;

  std::thread server_thread_;
  std::thread sender_thread_;
  std::deque<WebsocketMessage> send_queue_;
  std::map<websocketpp::connection_hdl,
           ClientParams,
           std::owner_less<websocketpp::connection_hdl>>
      client_hdl_map_;

  std::condition_variable var_send;
  std::mutex mut_send;
  TvbHandler* tvb_handler_ = nullptr;
  NestHandler* nest_handler_ = nullptr;
  bool runSendLoop_ = true;
};
}  // namespace insite
