#include "websocket_server.h"
#include "nest_handler.h"
#include "spdlog/spdlog.h"
#include "tvb_handler.h"
#include "websocketpp/frame.hpp"
#include <cstdint>

namespace insite {

void WebsocketServer::SetTvbHandler(TvbHandler* tvb_handler) {
  tvb_handler_ = tvb_handler;
}

void WebsocketServer::SetNestHandler(NestHandler* nest_handler) {
  nest_handler_ = nest_handler;
}

void WebsocketServer::BroadcastAll(const std::string& msg,
                                   ResourceFlag resource_flags) {
  for (const auto& connection : client_hdl_map_) {
    if (connection.second.resource_flags & resource_flags) {
      SPDLOG_DEBUG("Sending broadcast message to client {}: {}",
                   connection.second.id, msg);
      try {
        server_.send(connection.first, msg, websocketpp::frame::opcode::text);
      } catch (websocketpp::lib::error_code e) {
        SPDLOG_ERROR("Exception caught in BroadcastAll: {}", e.message());
      }
    }
  }
}

void WebsocketServer::StartServer() {
  server_.set_access_channels(websocketpp::log::alevel::all);
  server_.clear_error_channels(websocketpp::log::alevel::frame_payload);

  server_.init_asio();
  server_.set_max_message_size(kMaxMessageSize);
  server_.listen(port_);

  server_.set_open_handler(
      [this](const websocketpp::connection_hdl& hdl) { return OnOpen(hdl); });

  server_.set_close_handler(
      [this](const websocketpp::connection_hdl& hdl) { return OnClose(hdl); });

  server_.set_message_handler(
      [this](const websocketpp::connection_hdl& hdl,
             const Server::message_ptr& msg) { return OnMessage(hdl, msg); });

  server_.start_accept();
  server_thread_ = std::move(std::thread([this]() {
    SPDLOG_DEBUG("Starting websocket server on port: {}", port_);
    server_.run();
  }));

  sender_thread_ = std::move(std::thread([this]() { SendLoop(); }));
}

ResourceFlag WebsocketServer::GetResourceFlag(const std::string& resource) {
  if (resource == "/debug") {
    return ResourceFlag::kDebug;
  }
  if (resource == "/nest") {
    return ResourceFlag::kNest;
  }
  return ResourceFlag::kNone;
}

void WebsocketServer::StopServer() {
  runSendLoop_ = false;
  SPDLOG_DEBUG("Shutting down connections.");
  for (const auto& con : client_hdl_map_) {
    server_.close(con.first, websocketpp::close::status::going_away,
                  "Server shutdown");
  }
  server_.stop();
  server_thread_.join();
  sender_thread_.join();
}

void WebsocketServer::SendLoop() {
  while (runSendLoop_) {
    std::unique_lock mutex(mut_send);
    var_send.wait_for(mutex, 500ms, [&]() { return !(send_queue_.empty()); });

    while (!send_queue_.empty()) {
      const auto& message = send_queue_.front();
      if (message.receiver != 0) {
      } else {
        BroadcastAll(message.message, message.flags);
      }

      send_queue_.pop_front();
    }
  }
  SPDLOG_DEBUG("Finished WebsocketServer SendLoop");
}

void WebsocketServer::Send(WebsocketMessage&& message) {
  {
    std::lock_guard mutex(mut_send);
    send_queue_.push_back(std::move(message));
  }
  var_send.notify_all();
}

WebsocketServer ::WebsocketServer(std::uint64_t port) : port_(port) {}

void WebsocketServer::OnOpen(const websocketpp::connection_hdl& hdl) {
  auto con = server_.get_con_from_hdl(hdl);

  ClientParams params{};
  params.id = no_connections_++;
  params.type = PeerType::kClient;
  params.resource_flags = GetResourceFlag(con->get_resource());
  if (params.resource_flags == ResourceFlag::kNest) {
    con->set_message_handler([this](const websocketpp::connection_hdl& hdl,
                                    const Server::message_ptr& msg) {
      return OnMessageFromNESTServer(hdl, msg);
    });
  }

  SPDLOG_DEBUG(
      "[WebsocketServer::OnOpen] New Websocket Client connected on endpoint "
      "{0}: client id:{1}, flags: {2:b} ",
      con->get_remote_endpoint(), params.id,
      static_cast<std::uint8_t>(params.resource_flags));
  client_hdl_map_[hdl] = params;
}

void WebsocketServer::OnClose(const websocketpp::connection_hdl& hdl) {
  if (auto conn = client_hdl_map_.find(hdl); conn != client_hdl_map_.end()) {
    SPDLOG_DEBUG("[WebsocketServer::OnClose] Removing connection with id: {}",
                 conn->second.id);
    Send({"Client removed with id: " + std::to_string(conn->second.id), 0,
          ResourceFlag::kDebug});
    client_hdl_map_.erase(hdl);
  }
}

void WebsocketServer::OnMessage(const websocketpp::connection_hdl& hdl,
                                const Server::message_ptr& message) {
  if (tvb_handler_ != nullptr) {
    auto conn = client_hdl_map_.find(hdl);

    SPDLOG_DEBUG("Adding message from {} to tvb queue", conn->second.id);
    tvb_handler_->AddMessageIntoQueue(std::move(message->get_raw_payload()));
  }
}

void WebsocketServer::OnMessageFromNESTServer(
    const websocketpp::connection_hdl& hdl,
    const Server::message_ptr& message) {
  if (tvb_handler_ != nullptr) {
    nest_handler_->AddMessageIntoQueue(message->get_opcode(),
                                       std::move(message->get_raw_payload()));
  }
}

WebsocketServer ::~WebsocketServer() {}

}  // namespace insite
