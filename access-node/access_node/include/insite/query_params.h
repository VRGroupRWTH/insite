#pragma once

#include <crow/http_request.h>
#include <regex>
#include <string>
#include "optional.h"

template <typename T>
T ConvertStringToType(const char* input_string);

template <>
inline bool ConvertStringToType<bool>(const char* input_string) {
  return std::stoi(input_string) != 0;
}
template <>
inline int ConvertStringToType<int>(const char* input_string) {
  return std::stoi(input_string);
}

template <>
inline uint64_t ConvertStringToType(const char* input_string) {
  return std::stoull(input_string);
}

template <>
inline double ConvertStringToType(const char* input_string) {
  return std::stod(input_string);
}
template <>
inline std::string ConvertStringToType(const char* input_string) {
  return input_string;
}

template <typename T>
tl::optional<T> GetParameter(const crow::query_string& query_string,
                             const std::string& fieldname) {
  const auto parameter_value = query_string.get(fieldname);
  if (parameter_value == nullptr) {
    return tl::nullopt;
  }
  return ConvertStringToType<T>(parameter_value);
}

inline std::vector<std::uint64_t> CommaListToUintVector(std::string input) {
  auto regex = std::regex("((\\%2C|,)+)");
  std::vector<std::uint64_t> filter_node_ids;
  if (input == "")
    return filter_node_ids;
  std::sregex_token_iterator it{input.begin(), input.end(), regex, -1};
  std::vector<std::string> filter_gid_strings{it, {}};
  std::transform(filter_gid_strings.begin(), filter_gid_strings.end(),
                 std::back_inserter(filter_node_ids),
                 [](const std::string& str) { return std::stoll(str); });
  return filter_node_ids;
}

inline std::vector<std::uint64_t> CommaListToUintVector(std::string input,
                                                        std::regex regex) {
  std::vector<std::uint64_t> filter_node_ids;
  if (input == "")
    return filter_node_ids;
  std::sregex_token_iterator it{input.begin(), input.end(), regex, -1};
  std::vector<std::string> filter_gid_strings{it, {}};
  std::transform(filter_gid_strings.begin(), filter_gid_strings.end(),
                 std::back_inserter(filter_node_ids),
                 [](const std::string& str) { return std::stoll(str); });
  return filter_node_ids;
}

struct MultimeterParameter {
  MultimeterParameter(const crow::query_string& query_string) {
    from_time = GetParameter<double>(query_string, "fromTime").value_or(0.0);
    to_time = GetParameter<double>(query_string, "toTime")
                  .value_or(std::numeric_limits<double>::max());
    attribute = GetParameter<std::string>(query_string, "attribute");
    multimeter_id = GetParameter<std::uint64_t>(query_string, "multimeterId");

    auto parameter_gids = GetParameter<std::string>(query_string, "nodeIds");
    node_gids = parameter_gids.map_or(
        [](tl::optional<std::string> parameter_gids) {
          return CommaListToUintVector(parameter_gids.value());
        },
        std::vector<std::uint64_t>());
  }

 public:
  tl::optional<double> from_time;
  tl::optional<double> to_time;
  tl::optional<std::string> attribute;
  std::vector<std::uint64_t> node_gids;
  tl::optional<std::uint64_t> multimeter_id;
};

struct SpikeParameter {
  SpikeParameter(const crow::query_string& query_string) {
    from_time = GetParameter<double>(query_string, "fromTime").value_or(0.0);
    to_time = GetParameter<double>(query_string, "toTime")
                  .value_or(std::numeric_limits<double>::max());

    skip = GetParameter<int>(query_string, "skip");
    top = GetParameter<int>(query_string, "top");
    sort = GetParameter<bool>(query_string, "sort");
    node_collection_id =
        GetParameter<uint64_t>(query_string, "nodeCollectionId");
    spike_detector_id = GetParameter<uint64_t>(query_string, "spikedetectorId");
    auto parameter_gids = GetParameter<std::string>(query_string, "nodeIds");
    node_gids =
        parameter_gids.map([](tl::optional<std::string> parameter_gids) {
          return CommaListToUintVector(parameter_gids.value());
        });
  }

 public:
  tl::optional<double> from_time;
  tl::optional<double> to_time;
  tl::optional<int> skip;
  tl::optional<int> top;
  tl::optional<bool> sort;
  tl::optional<std::uint64_t> node_collection_id;
  tl::optional<std::uint64_t> spike_detector_id;
  tl::optional<std::vector<std::uint64_t>> node_gids;
};
