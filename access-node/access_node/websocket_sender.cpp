#include "websocket_sender.h"
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/ranges.h>
#include <cstdint>
#include <string>
#include <websocketpp/server.hpp>
#include "opcodes.h"
#include "resource_flags.h"
#include "spdlog/spdlog.h"
#include "websocketpp/common/system_error.hpp"
#include "websocketpp/frame.hpp"

namespace insite {

void WebsocketSender::Consumer(std::vector<WebsocketMessage>* queue) {
  while (true) {
    std::unique_lock mutex(mut_);
    var_.wait(mutex, [&]() { return !(queue->empty()); });

    while (!queue->empty()) {
      const auto& message = queue->back();
      if (message.receiver != 0) {
      } else {
        BroadcastAll(message.message, message.flags);
      }

      queue->pop_back();
    }
  }
}

void WebsocketSender::BroadcastAll(const std::string& msg,
                                   ResourceFlag resource) {
  for (const auto& connection : connection_map) {
    if (connection.second.resource_flags & resource) {
      // spdlog::debug("Sending broadcast message to client {}: {}",
      //               connection.second.id, msg);
      try {
        websocket_server.send(connection.first, msg,
                              websocketpp::frame::opcode::text);
      } catch (websocketpp::lib::error_code e) {
        spdlog::error("Exception caught in BroadcastAll: {}", e.message());
      }
    }
  }
}

}  // namespace insite
