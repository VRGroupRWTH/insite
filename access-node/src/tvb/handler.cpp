#include <insite_defines.h>

#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/ranges.h>
#include <tvb/handler.h>
#include <chrono>
#include <cstdint>
#include "opcodes.h"
#include "spdlog/spdlog.h"
using namespace std::chrono_literals;

namespace insite {

void TvbHandler::SerializeMonitorsJson(
    rapidjson::Writer<rapidjson::StringBuffer>& writer) {
  writer.StartArray();
  for (auto& monitor : double_monitors_) {
    monitor.SerializeMetadataToJson(writer);
  }
  writer.EndArray();
}

void TvbHandler::AddMessageIntoQueue(std::string&& msg) {
  {
    std::lock_guard mutex(mut_);
    message_queue_.push_back(std::move(msg));
    // SPDLOG_DEBUG("Added msg to TvbHandler");
  }
  var_.notify_one();
}

void TvbHandler::Consumer() {
  SPDLOG_DEBUG("Started TvbHandler Consumer Thread");
  while (runConsumerLoop_) {
    std::unique_lock mutex(mut_);
    var_.wait_for(mutex, 500ms, [&]() { return !(message_queue_.empty()); });

    // SPDLOG_DEBUG("Consuming message from tvb handler");
    while (!message_queue_.empty()) {
      Opcode opcode = Opcode::kUndefined;
      std::string& data = message_queue_.back();
      std::memcpy(&opcode, data.data(), sizeof(Opcode));
      // SPDLOG_DEBUG(opcode);

      rapidjson::Document doc;
      switch (opcode) {
        case Opcode::kEndSim:
          SPDLOG_TRACE("TVB Simulation has ended. Bytes: {}",
                       bytes / (1000 * 1000));
          break;
        case Opcode::kStartSim:
          SPDLOG_TRACE("New TVB Simulation started");
          double_monitors_.clear();
          break;
        case Opcode::kRegisterNewMonitor:
          ParseNewMonitorPacket(data);
          break;
        case Opcode::kData:
          SPDLOG_TRACE("New TVB Data");
          ParseDataPacket(data);
          break;
        case Opcode::kSimInfo:
          SPDLOG_TRACE("Parse Sim Info");
          ParseSimInfo(data);
          break;

        case Opcode::kSimInfoReturn:
          SPDLOG_TRACE("Parse Sim Info Return");
          ParseSimInfoReturn(data);
          break;
        default:
          SPDLOG_ERROR("[TVB Handler] Unknown opcode: " + std::to_string(opcode) +
                           " Message: {}",
                       std::string(data));
      }

      message_queue_.pop_back();
    }
  }
  SPDLOG_DEBUG("Finished TvbHandler Consumer Thread");
}
void TvbHandler::ParseSimInfo(const std::string& payload) {
  sim_info.Parse(payload.c_str() + 1);
  // spdlog::error(payload);
}

void TvbHandler::ParseSimInfoReturn(const std::string& payload) {
  // spdlog::error("ParseSimInfoReturn: {}", payload);
  // sim_info.Parse(payload.c_str() + 1);
  sim_info_promise.set_value(payload.substr(1));
  // spdlog::error(payload);
}

void TvbHandler::ParseNewMonitorPacket(const std::string& payload) {
  rapidjson::Document doc;
  doc.Parse((payload.c_str() + 1));
  const auto* monitor_name = doc["name"].GetString();
  const auto* monitor_gid = doc["gid"].GetString();
  const auto observed_vars_array = doc["observedVariables"].GetArray();
  const auto internal_id = doc["internalId"].GetInt();
  std::vector<std::string> observed_vars;
  for (const auto& var : observed_vars_array) {
    observed_vars.emplace_back(var.GetString());
  }

  number_of_monitors++;
  double_monitors_.emplace_back(monitor_name, internal_id, monitor_gid,
                                observed_vars);

  SPDLOG_DEBUG("Register New Monitor {}", payload);
  SPDLOG_DEBUG("Registered Monitors");
  for (const auto& monitor : double_monitors_) {
    SPDLOG_DEBUG("   {}", monitor);
  }
}

void TvbHandler::ParseDataPacket(const std::string& payload) {
  bytes += payload.length();
  std::uint64_t cursor = sizeof(Opcode);
  DataHeader header{};
  std::memcpy(&header, payload.data() + sizeof(Opcode), sizeof(header));
  cursor += sizeof(DataHeader);
  if (header.type == DataType::kDouble) {
    const auto* dat = reinterpret_cast<const double*>(payload.data() + cursor);
    double_monitors_[header.internal_id].data.PushBack(
        header, dat, (const double*)(&(payload.back()) + 1));
  }
}

}  // namespace insite
