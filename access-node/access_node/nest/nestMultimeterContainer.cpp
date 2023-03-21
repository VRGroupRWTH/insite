#include "jsonStrings.h"
#include <algorithm>
#include <nest/nestMultimeterContainer.h>

namespace insite {
void MultimeterContainer::SerializeToJson(
    rapidjson::Writer<rapidjson::StringBuffer> &writer) const {
  writer.StartObject();
  writer.Key(json_strings::kMultimeterId);
  writer.Uint64(identifier_);

  writer.Key(json_strings::kAttributes);
  writer.StartArray();
  for (const auto &attribute : attributes_) {
    writer.String(attribute.c_str());
  }
  writer.EndArray();

  writer.Key(json_strings::kNodeIds);
  writer.StartArray();
  for (const auto node_id : node_ids_) {
    writer.Uint64(node_id);
  }
  writer.EndArray();

  writer.EndObject();
}

void MultimeterContainer::AddDataFromJson(
    rapidjson::Value &multimeter_json_value) {
  identifier_ = multimeter_json_value[json_strings::kMultimeterId].GetUint64();

  for (auto &attribute :
       multimeter_json_value[json_strings::kAttributes].GetArray()) {
    attributes_.insert(attribute.GetString());
  }

  for (auto &node_id :
       multimeter_json_value[json_strings::kNodeIds].GetArray()) {
    node_ids_.emplace_back(node_id.GetUint64());
  }
}

MultimeterValueContainer::MultimeterValueContainer(
    std::vector<uint64_t> node_ids)
    : node_ids_(std::move(node_ids)) {
  std::sort(node_ids_.begin(), node_ids_.end());
}

void MultimeterValueContainer::AddDataFromJsonV2(
    rapidjson::Value &multimeter_values) {
  std::vector<std::size_t> strides;

  if (simulation_times_.empty()) {
    for (auto &time : multimeter_values[json_strings::kSimTimes].GetArray()) {
      simulation_times_.push_back(time.GetDouble());
    }
    values_.resize(simulation_times_.size() * node_ids_.size());
  }

  auto node_id_array = multimeter_values[json_strings::kNodeIds].GetArray();
  for (auto &node_id : node_id_array) {
    auto node_id_position =
        std::find(node_ids_.begin(), node_ids_.end(), node_id.GetUint64());
    auto stride = std::distance(node_ids_.begin(), node_id_position);
    strides.push_back(stride);
  }

  auto sim_time_length = simulation_times_.size();
  auto value_array = multimeter_values[json_strings::kValues].GetArray();
  for (rapidjson::SizeType index = 0; index < value_array.Size(); index++) {
    std::ldiv_t div_result = std::div((long)index, (long)node_id_array.Size());
    auto offset = div_result.quot * node_ids_.size() + strides[div_result.rem];
    values_[offset] = value_array[index].GetDouble();
  }
}

void MultimeterValueContainer::AddDataFromJson(
    rapidjson::Value &multimeter_values) {
  std::vector<std::size_t> strides;

  if (simulation_times_.empty()) {
    for (auto &time : multimeter_values[json_strings::kSimTimes].GetArray()) {
      simulation_times_.push_back(time.GetDouble());
    }
    values_.resize(simulation_times_.size() * node_ids_.size());
  }

  auto node_id_array = multimeter_values[json_strings::kNodeIds].GetArray();
  for (auto &node_id : node_id_array) {
    auto node_id_position =
        std::find(node_ids_.begin(), node_ids_.end(), node_id.GetUint64());
    auto stride = std::distance(node_ids_.begin(), node_id_position);
    strides.push_back(stride);
  }

  auto sim_time_length = simulation_times_.size();
  auto value_array = multimeter_values[json_strings::kValues].GetArray();
  for (rapidjson::SizeType index = 0; index < value_array.Size(); index++) {
    std::ldiv_t div_result = std::div((long)index, (long)node_id_array.Size());
    auto offset = div_result.quot * node_ids_.size() + strides[div_result.rem];
    values_[offset] = value_array[index].GetDouble();
  }
}

void MultimeterValueContainer::SerializeToJsonV2(
    rapidjson::Writer<rapidjson::StringBuffer> &writer) {
  writer.StartObject();
  writer.Key("simId");
  writer.String(sim_id.GetString());
  writer.Key(json_strings::kNodeIds);
  writer.StartArray();
  for (auto &node_id : node_ids_) {
    writer.Uint64(node_id);
  }
  writer.EndArray();

  writer.Key(json_strings::kSimTimes);
  writer.StartArray();
  for (auto &sim_time : simulation_times_) {
    writer.Double(sim_time);
  }
  writer.EndArray();

  writer.Key(json_strings::kValues);
  writer.StartArray();
  for (auto &value : values_) {
    writer.Double(value);
  }
  writer.EndArray();

  writer.EndObject();
}

void MultimeterValueContainer::SerializeToJson(
    rapidjson::Writer<rapidjson::StringBuffer> &writer) {
  writer.StartObject();
  writer.Key(json_strings::kNodeIds);
  writer.StartArray();
  for (auto &node_id : node_ids_) {
    writer.Uint64(node_id);
  }
  writer.EndArray();

  writer.Key(json_strings::kSimTimes);
  writer.StartArray();
  for (auto &sim_time : simulation_times_) {
    writer.Double(sim_time);
  }
  writer.EndArray();

  writer.Key(json_strings::kValues);
  writer.StartArray();
  for (auto &value : values_) {
    writer.Double(value);
  }
  writer.EndArray();

  writer.EndObject();
}
} // namespace insite

template <> struct fmt::formatter<insite::MultimeterValueContainer> {
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const insite::MultimeterValueContainer &multimeter,
              FormatContext &ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(
        ctx.out(),
        "\nMultimeterValues: [\nnodes:\n {}\nsimTimes: {} \nvalues:\n {}",
        multimeter.node_ids_, multimeter.simulation_times_, multimeter.values_);
  }
};

template <> struct fmt::formatter<insite::MultimeterContainer> {
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const insite::MultimeterContainer &multimeter,
              FormatContext &ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(
        ctx.out(),
        "\nMultimeter: [\nmultimeterId:\n {}\nnodes: {} \nattributes:\n {}",
        multimeter.identifier_, multimeter.node_ids_, multimeter.attributes_);
  }
};
