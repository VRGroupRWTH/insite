#pragma once
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <iterator>
#include <optional>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <vector>

#include <nest/nestSpikes.h>
#include <utilityFunctions.h>
namespace insite {

using NodeList = std::vector<uint64_t>;

class Spikerecorder {
public:
  Spikerecorder();
  Spikerecorder(const rapidjson::Value &document);
  Spikerecorder(const rapidjson::Value &document, std::string sim_id);
  Spikerecorder(Spikerecorder &&) = default;
  Spikerecorder(const Spikerecorder &) = default;
  Spikerecorder &operator=(Spikerecorder &&) = default;
  Spikerecorder &operator=(const Spikerecorder &) = default;
  ~Spikerecorder();
  void ParseFromJson(rapidjson::Document input);
  void WriteToJson(rapidjson::Writer<rapidjson::StringBuffer> &writer) const;
  friend std::ostream &operator<<(std::ostream &, const Spikerecorder &);
  std::string sim_id;
  uint64_t spikerecorder_id;
  NodeList nodes;
};

}; // namespace insite
template <> struct fmt::formatter<insite::Spikerecorder> {
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const insite::Spikerecorder &spikerecorder,
              FormatContext &ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(),
                          "Spikerecorder: [ spikedetecorId: {}, nodes:{}",
                          spikerecorder.spikerecorder_id, spikerecorder.nodes);
  }
};
