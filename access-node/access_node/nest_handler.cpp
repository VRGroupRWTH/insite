#include "nest_handler.h"
#include <rapidjson/document.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/ranges.h>
#include <chrono>
#include <cstdint>
#include "flatbuffer-schema/schema_generated.h"
#include "opcodes.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "spdlog/spdlog.h"
#include "spike_util.h"
#include "websocket_server.h"
#include "websocketpp/frame.hpp"
using namespace std::chrono_literals;

namespace insite {

void NestHandler::AddMessageIntoQueue(websocketpp::frame::opcode::value value,
                                      std::string&& msg) {
  {
    std::lock_guard mutex(mut_);
    message_queue_.emplace_back(value, std::move(msg));
  }
  var_.notify_one();
}

void NestHandler::Consumer() {
  spdlog::debug("Started NestHandler Consumer Thread");
  while (runConsumerLoop_) {
    std::unique_lock mutex(mut_);
    var_.wait_for(mutex, 500ms, [&]() { return !(message_queue_.empty()); });

    while (!message_queue_.empty()) {
      if (message_queue_.back().first == websocketpp::frame::opcode::BINARY) {
        const Insite::Nest::SpikeTable* spike_table =
            Insite::Nest::GetSpikeTable(message_queue_.back().second.c_str());
        double lastTimestamp = 0;
        if (spike_table->spikes()->size() > 0) {
          lastTimestamp = spike_table->spikes()->rbegin()->spike_time();
        }
        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
        writer.StartObject();
        writer.Key("type");
        writer.Uint(1);
        writer.Key("data");
        FlatbufferSpikeTableToJsonSpikeArray(spike_table, writer);
        writer.Key("lastTimestamp");
        writer.Double(lastTimestamp);
        writer.EndObject();
        spdlog::debug(
            "[NestHandler::Consumer] Received {} spikes with latest timestamp: "
            "{}",
            spike_table->spikes()->size(), lastTimestamp);
        try {
          ws_server->Send({buf.GetString(), 0, ResourceFlag::kDebug});
        } catch (websocketpp::lib::error_code e) {
          spdlog::error("Exception caught in Nest Consumer: {}", e.message());
        }
      } else {
        spdlog::debug("[NestHandler::Consumer] Received {}",
                      message_queue_.back().second);
        try {
          ws_server->Send(
              {message_queue_.back().second, 0, ResourceFlag::kDebug});
        } catch (websocketpp::lib::error_code e) {
          spdlog::error("Exception caught in Nest Consumer: {}", e.message());
        }
      }

      message_queue_.pop_back();
    }
  }
  spdlog::debug("Finished NestHandler Consumer Thread");
}

}  // namespace insite
