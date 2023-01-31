#include "config.h"
#include <spdlog/spdlog.h>
#include <toml++/toml.h>
#include <yaml-cpp/yaml.h>
#include <iterator>
#include <string_view>
#include <type_traits>
#include <vector>
#include "filesystem"
#include "yaml-cpp/node/parse.h"

namespace insite {

using VecInt = int;

template <typename>
struct IsStdVector : std::false_type {};

template <typename T, typename A>
struct IsStdVector<std::vector<T, A>> : std::true_type {};

template <typename T>
auto ParseIfFieldExists(const YAML::Node& config,
                        const std::string& field_name,
                        T default_value) -> T {
  if (auto field_node = config[field_name]; field_node) {
    if constexpr (IsStdVector<T>::value) {
      if (field_node.IsSequence()) {
        auto field_value = field_node.as<T>();
        return field_value;
      }
      auto field_value = field_node.as<typename T::value_type>();
      return {field_value};
    } else {
      T field_value = field_node.as<T>();
      return field_value;
    }
  } else {
    return default_value;
  }
}

void ServerConfig::ParseYamlConfigFromString(const std::string& content) {
  YAML::Node config = YAML::Load(content);

  // NEST Variables
  number_of_nodes =
      ParseIfFieldExists<int>(config["nest"], "numberOfNodes", number_of_nodes);

  base_url =
      ParseIfFieldExists<std::string>(config["nest"], "baseUrl", base_url);

  port_number_nodes =
      ParseIfFieldExists<int>(config["nest"], "ports", port_number_nodes);

  ports_consecutive = ParseIfFieldExists<bool>(
      config["nest"], "portsConsecutive", ports_consecutive);

  same_base_url =
      ParseIfFieldExists<bool>(config["nest"], "sameBaseUrl", same_base_url);

  // Access Node Variables
  port_number_access =
      ParseIfFieldExists<int>(config["accessNode"], "port", port_number_access);

  port_number_access_ws = ParseIfFieldExists<int>(
      config["accessNode"], "websocketPort", port_number_access_ws);
}

void ServerConfig::ParseYamlConfigFromFile(const std::string& filename) {
  std::ifstream fin(filename);
  ParseYamlConfigFromString(std::string(std::istreambuf_iterator<char>(fin),
                                        std::istreambuf_iterator<char>()));
}

void ServerConfig::ParseTomlConfigFromFile(const std::string& filename) {
  auto config = toml::parse_file(filename);
  std::stringstream yaml;
  yaml << toml::yaml_formatter(config);
  SPDLOG_DEBUG("[ServerConfig::ParseTomlConfigFromFile] Toml as Yaml:\n {}",
               yaml.str());
  ParseYamlConfigFromString(yaml.str());
}

void ServerConfig::GenerateRequestUrls() {
  if (ports_consecutive && same_base_url) {
    for (int i = 0; i < number_of_nodes; ++i) {
      std::string url("http://" + base_url + ":" +
                      std::to_string(port_number_nodes + i));
      request_urls.push_back(url);
      SPDLOG_DEBUG(
          "[ServerConfig::GenerateRequestUrls] Added {} to the request urls",
          url);
    }
  }
}

std::string ServerConfig::ToString() const {
  return fmt::format(
      "Server Config:\n"
      "Simulation Nodes #: {}\n"
      "Simulation Nodes Ports: {}\n"
      "Simulation Nodes URLs: {}\n"
      "Simulation Nodes Same Base URL: {}\n"
      "Simulation Nodes Ports Consecutive: {}\n"
      "Access Node Port: {}\n",
      "Access Node Websocket Port: {}\n", number_of_nodes, port_number_nodes,
      base_url, same_base_url, ports_consecutive, port_number_access,
      port_number_access_ws);
}

void ServerConfig::ParseConfigIfExists() {
  if (std::filesystem::exists("config.yml")) {
    ParseYamlConfigFromFile("config.yml");
  } else if (std::filesystem::exists("config.yaml")) {
    ParseYamlConfigFromFile("config.yaml");
  } else if (std::filesystem::exists("config.toml")) {
    ParseTomlConfigFromFile("config.toml");
  }

  SPDLOG_DEBUG("{}", ToString());
}

}  // namespace insite

template <>
struct fmt::formatter<insite::ServerConfig> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const insite::ServerConfig& config, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    // clang-format off
    return fmt::format_to(ctx.out(),config.ToString());
    // clang-format on
  }
};
