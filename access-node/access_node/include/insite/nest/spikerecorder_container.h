#pragma once
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <nest/spikes.h>
#include <utility_functions.h>
namespace insite {

using NodeList = std::vector<uint64_t>;

class SpikerecorderContainer {

 public:
  SpikerecorderContainer();
  SpikerecorderContainer(const rapidjson::Value& document);
  SpikerecorderContainer(const rapidjson::Value& document, std::string sim_id);
  SpikerecorderContainer(SpikerecorderContainer&&) = default;
  SpikerecorderContainer(const SpikerecorderContainer&) = default;
  SpikerecorderContainer& operator=(SpikerecorderContainer&&) = default;
  SpikerecorderContainer& operator=(const SpikerecorderContainer&) = default;
  ~SpikerecorderContainer();
  void ParseFromJson(rapidjson::Document input);
  void WriteToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;
  friend std::ostream& operator<<(std::ostream&, const SpikerecorderContainer&);
  std::string sim_id;
  uint64_t spikerecorder_id;
  NodeList nodes;
};

};  // namespace insite

template <>
struct fmt::formatter<insite::SpikerecorderContainer> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const insite::SpikerecorderContainer& spikerecorder, FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "Spikerecorder: [ spikedetecorId: {}, nodes:{}", spikerecorder.spikerecorder_id, spikerecorder.nodes);
  }
};
