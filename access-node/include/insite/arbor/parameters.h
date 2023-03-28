#pragma once

#include <crow.h>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <regex>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <string>
#include <tl/optional.hpp>

namespace insite {

template <typename T> T ConvertStringToType(const char *input_string);

template <> inline bool ConvertStringToType<bool>(const char *input_string) {
  return std::stoi(input_string) != 0;
}

template <> inline int ConvertStringToType<int>(const char *input_string) {
  return std::stoi(input_string);
}

template <> inline uint64_t ConvertStringToType(const char *input_string) {
  return std::stoull(input_string);
}

template <> inline double ConvertStringToType(const char *input_string) {
  return std::stod(input_string);
}

template <> inline std::string ConvertStringToType(const char *input_string) {
  return input_string;
}

inline std::vector<std::uint64_t> CommaListToUintVector(std::string input) {
  auto regex = std::regex("((\\%2C|,)+)");
  std::vector<std::uint64_t> filter_node_ids;
  if (input.empty()) {
    return filter_node_ids;
  }
  std::sregex_token_iterator it{input.begin(), input.end(), regex, -1};
  std::vector<std::string> filter_gid_strings{it, {}};
  std::transform(filter_gid_strings.begin(), filter_gid_strings.end(),
                 std::back_inserter(filter_node_ids),
                 [](const std::string &str) { return std::stoll(str); });
  return filter_node_ids;
}

inline std::vector<std::uint64_t>
CommaListToUintVector(std::string input, const std::regex &regex) {
  std::vector<std::uint64_t> filter_node_ids;
  if (input.empty()) {
    return filter_node_ids;
  }
  std::sregex_token_iterator it{input.begin(), input.end(), regex, -1};
  std::vector<std::string> filter_gid_strings{it, {}};
  std::transform(filter_gid_strings.begin(), filter_gid_strings.end(),
                 std::back_inserter(filter_node_ids),
                 [](const std::string &str) { return std::stoll(str); });
  return filter_node_ids;
}

template <typename T>
tl::optional<T> GetParameter(const crow::query_string &query_string,
                             const std::string &fieldname) {
  auto *const parameter_value = query_string.get(fieldname);
  if (parameter_value == nullptr) {
    return tl::nullopt;
  }
  auto value = ConvertStringToType<T>(parameter_value);

  SPDLOG_TRACE("Converted Parameter: {} with value: {}", fieldname, value);
  return value;
}

template <typename T>
std::vector<T> GetListParameter(const crow::query_string &query_string,
                                const std::string &fieldname) {
  std::vector<T> value;
  const auto parameter_value = query_string.get_list(fieldname, false);
  if (parameter_value.empty()) {
    return value;
  }

  value.reserve(parameter_value.size());
  std::transform(parameter_value.begin(), parameter_value.end(),
                 std::back_inserter(value), [](const std::string &str) {
                   return ConvertStringToType<T>(str.c_str());
                 });

  SPDLOG_TRACE("Converted Parameter: {} with value: {}", fieldname, value);
  return value;
}

struct SpikeParameter {

  SpikeParameter(const crow::query_string &params) {

    g_id = GetListParameter<int>(params, "gId");
    from_time = GetParameter<double>(params, "fromTime");
    to_time = GetParameter<double>(params, "toTime");
  }

public:
  std::vector<int> g_id;
  tl::optional<double> from_time;
  tl::optional<double> to_time;
};

struct ProbeParameter {

  ProbeParameter(const crow::query_string &params) {

    g_id = GetParameter<int>(params, "gId");
    l_id = GetParameter<int>(params, "lId");
    p_id = GetParameter<int>(params, "pId");
    u_id = GetListParameter<int>(params, "uId");
    hash = GetListParameter<std::uint64_t>(params, "hash");
    from_time = GetParameter<double>(params, "fromTime");
    to_time = GetParameter<double>(params, "toTime");
  }

public:
  tl::optional<int> g_id;
  tl::optional<int> l_id;
  tl::optional<int> p_id;
  std::vector<std::uint64_t> hash;
  std::vector<int> u_id;
  tl::optional<double> from_time;
  tl::optional<double> to_time;
};

} // namespace insite
