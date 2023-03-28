#pragma once

#include "spdlog/spdlog.h"
#include "string_util.h"
#include <crow/http_request.h>
#include <regex>
#include <string>
#include <tl/optional.hpp>
using Parameter = std::pair<std::string, std::string>;

template <typename T>
tl::optional<T> GetParameter(const crow::query_string &query_string,
                             const std::string &fieldname) {
  const auto parameter_value = query_string.get(fieldname);
  if (parameter_value == nullptr) {
    return tl::nullopt;
  }
  return ConvertStringToType<T>(parameter_value);
}

template <typename T>
inline std::string VectorToCsv(const std::vector<T> &values) {
  std::string ret;

  for (auto it = values.begin(); it != values.end(); ++it) {
    if (it != values.begin()) {
      ret += ",";
    }
    ret += std::to_string(*it);
  }

  return ret;
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
                 [](const std::string &str) { return std::stoll(str); });
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
                 [](const std::string &str) { return std::stoll(str); });
  return filter_node_ids;
}

struct TvbParameters {
  TvbParameters(const crow::query_string &query_string) {
    from_time = GetParameter<double>(query_string, "fromTime");
    to_time = GetParameter<double>(query_string, "toTime");
    internal_id = GetParameter<double>(query_string, "internalId");
    uid = GetParameter<std::string>(query_string, "uid");
  }

public:
  tl::optional<double> from_time;
  tl::optional<double> to_time;
  tl::optional<int> internal_id;
  tl::optional<std::string> uid;
};

struct MultimeterParameter {
  MultimeterParameter(const crow::query_string &query_string) {
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

  [[nodiscard]] std::vector<Parameter> GetParameterVector() const {
    std::vector<Parameter> params;

    from_time.map([&params](auto val) {
      params.emplace_back("fromTime", std::to_string(val));
    });

    to_time.map([&params](auto val) {
      params.emplace_back("toTime", std::to_string(val));
    });

    attribute.map(
        [&params](auto val) { params.emplace_back("attribute", val); });

    if (!node_gids.empty()) {
      params.emplace_back("nodeIds", VectorToCsv(node_gids));
    }

    multimeter_id.map([&params](auto val) {
      params.emplace_back("multimeterId", std::to_string(val));
    });

    return params;
  }

public:
  tl::optional<double> from_time;
  tl::optional<double> to_time;
  tl::optional<std::string> attribute;
  std::vector<std::uint64_t> node_gids;
  tl::optional<std::uint64_t> multimeter_id;
};

struct SpikeParameter {
  SpikeParameter(const crow::query_string &query_string) {
    from_time = GetParameter<double>(query_string, "fromTime");
    to_time = GetParameter<double>(query_string, "toTime");

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
    auto order = GetParameter<std::string>(query_string, "order");
    if (order == "desc") {
      reverse_order = true;
    } else {
      reverse_order = false;
    }
  }

  [[nodiscard]] std::vector<Parameter> GetParameterVector() const {
    std::vector<Parameter> params;

    if (from_time) {
      params.emplace_back("fromTime", std::to_string(from_time.value()));
    }

    if (to_time) {
      params.emplace_back("toTime", std::to_string(to_time.value()));
    }

    if (skip) {
      params.emplace_back("skip", std::to_string(skip.value()));
    }

    if (sort) {
      params.emplace_back("sort",
                          std::to_string(static_cast<int>(sort.value())));
    }

    if (node_collection_id) {
      params.emplace_back("nodeCollectionId",
                          std::to_string(node_collection_id.value()));
    }

    if (spike_detector_id) {
      params.emplace_back("spikedetectorId",
                          std::to_string(spike_detector_id.value()));
    }

    if (node_gids) {
      params.emplace_back("nodeIds", VectorToCsv(node_gids.value()));
    }

    return params;
  }

public:
  tl::optional<double> from_time;
  tl::optional<double> to_time;
  tl::optional<int> skip;
  tl::optional<int> top;
  tl::optional<bool> sort;
  tl::optional<bool> reverse_order;
  tl::optional<std::uint64_t> node_collection_id;
  tl::optional<std::uint64_t> spike_detector_id;
  tl::optional<std::vector<std::uint64_t>> node_gids;
};
