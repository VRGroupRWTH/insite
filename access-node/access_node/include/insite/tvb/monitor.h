#pragma once
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <cstdint>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "circular_timeseries.h"
// #include "params.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"
#include "tl/optional.hpp"

namespace insite {

template <typename T>
class TvbMonitor {
  using It = typename std::vector<T>::iterator;

 public:
  uint32_t internal_id;
  std::string name;
  std::string uid;
  std::vector<std::string> observed_variables;

  TvbMonitor(std::string name, uint32_t internal_id, std::string uid, std::vector<std::string> observed_variables)
      : name(std::move(name)), internal_id(internal_id), uid(std::move(uid)), data(10, {1, 76, 2}), observed_variables(std::move(observed_variables)) {
    stride_length.push_back(1);
    stride_length.push_back(76);
    stride_length.push_back(2);
    SPDLOG_DEBUG(stride_length);
  };

  void
  SerializeMetadataToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
    writer.StartObject();
    writer.Key("uid");
    writer.String(uid.c_str());
    writer.Key("interalId");
    writer.Int(internal_id);
    writer.Key("type");
    writer.String(name.c_str());
    writer.Key("observedVariables");
    writer.StartArray();
    for (const auto& var : observed_variables) {
      writer.String(var.c_str());
    }
    writer.EndArray();
    writer.EndObject();
  }

  void SerializeDataToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer,
                           tl::optional<double> from_time,
                           tl::optional<double> to_time) {
    writer.StartObject();
    writer.Key("uid");
    writer.String(uid.c_str());
    writer.Key("timesteps");
    writer.StartArray();
    for (int timestep = 0; timestep < data.Size(); ++timestep) {
      auto time = data.GetTimeByIndex(timestep);
      if (!(time >= from_time.value_or(0) &&
            time <= to_time.value_or(std::numeric_limits<double>::max()))) {
        continue;
      }
      writer.StartObject();
      writer.Key("time");
      writer.Double(time);
      for (int var = 0; var < observed_variables.size(); var++) {
        writer.Key(observed_variables[var].c_str());
        writer.StartArray();
        auto res = data.GetVarByIndex(timestep, var);
        for (auto& data : res) {
          if constexpr (std::is_same_v<T, double>) {
            writer.Double(data);
          }
        }
        writer.EndArray();
      }
      writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();
  }

  CircularTimeSeries<T> data;
  std::vector<int> stride_length;

  std::tuple<It, It> GetDataByIndex(uint64_t index, uint8_t var) {
    int stride_product = stride_length[0] * stride_length[1] * stride_length[2];
    int start = index * stride_product + var * stride_length[1];
    int end = start + stride_length[1];
    return std::make_pair(data.begin() + start, data.begin() + end);
  };
};

}  // namespace insite

template <typename T>
struct fmt::formatter<insite::TvbMonitor<T>> {
  constexpr auto parse(format_parse_context& ctx)  // NOLINT
      -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const insite::TvbMonitor<T>& monitor,  // NOLINT
              FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "Monitor {} Name: {}, Observed Vars: {}",
                          monitor.uid, monitor.name,
                          monitor.observed_variables);
  }
};
