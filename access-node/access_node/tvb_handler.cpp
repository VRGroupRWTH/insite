#include "tvb_handler.h"
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/ranges.h>
#include <cstdint>
#include "opcodes.h"
#include "spdlog/spdlog.h"
#include <chrono>
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
    var_.wait_for(mutex, 500ms ,[&]() { return !(message_queue_.empty()); });

    // SPDLOG_DEBUG("Consuming message from tvb handler");
    while (!message_queue_.empty()) {
      Opcode opcode = Opcode::kUndefined;
      std::string& data = message_queue_.back();
      std::memcpy(&opcode, data.data(), sizeof(Opcode));
      SPDLOG_DEBUG(opcode);

      rapidjson::Document doc;
      switch (opcode) {
        case Opcode::kEndSim:
          SPDLOG_DEBUG("TVB Simulation has ended. Bytes: {}",
                       bytes / (1000 * 1000));
          break;
        case Opcode::kStartSim:
          SPDLOG_DEBUG("New TVB Simulation started");
          double_monitors_.clear();
          break;
        case Opcode::kRegisterNewMonitor:
          ParseNewMonitorPacket(data);
          break;
        case Opcode::kData:
          ParseDataPacket(data);
          break;
        default:
          SPDLOG_DEBUG("[TVB Handler] Unknown opcode: " +
                           std::to_string(opcode) + " Message: {}",
                       std::string(data));
      }

      message_queue_.pop_back();
    }
  }
  SPDLOG_DEBUG("Finished TvbHandler Consumer Thread");
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
  // SPDLOG_DEBUG(
  //     "Header: internalId: {}, time: {}, type: {}, dim1: {}, dim2: {}, dim3:
  //     "
  //     "{}",
  //     header.internal_id, header.time, header.type, header.dim1, header.dim2,
  //     header.dim3);
  if (header.type == DataType::kDouble) {
    const auto* dat = reinterpret_cast<const double*>(payload.data() + cursor);
    double_monitors_[header.internal_id].data.PushBack(
        header.time, dat, (const double*)(&(payload.back()) + 1));
  }
}

}  // namespace insite
