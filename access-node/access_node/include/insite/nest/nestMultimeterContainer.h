#include <spdlog/fmt/fmt.h>
#include <cstdint>
#include <optional>
#include <unordered_set>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace insite {

class MultimeterContainer {
 public:
  void AddDataFromJson(rapidjson::Value& multimeter_json_value);
  void SerializeToJson(
      rapidjson::Writer<rapidjson::StringBuffer>& writer) const;

  uint64_t identifier_;
  std::vector<uint64_t> node_ids_;
  std::unordered_set<std::string> attributes_;
};

class MultimeterValueContainer {
 public:
  void AddDataFromJson(rapidjson::Value& multimeter_values);
  void SerializeToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer);
  MultimeterValueContainer(std::vector<uint64_t> node_ids);

 private:
  std::vector<uint64_t> node_ids_;
  std::vector<double> simulation_times_;
  std::vector<double> values_;

  friend struct fmt::formatter<MultimeterValueContainer>;
};
}  // namespace insite


