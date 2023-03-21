#include "config.h"
#include "filesystem"
#include "string_util.h"
#include "tl/optional.hpp"
#include "yaml-cpp/node/parse.h"
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <spdlog/spdlog.h>
#include <string_view>
#include <toml++/toml.h>
#include <type_traits>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace insite {

using VecInt = int;

template <typename> struct IsStdVector : std::false_type {};

template <typename T, typename A>
struct IsStdVector<std::vector<T, A>> : std::true_type {};

template <typename T>
auto ParseIfFieldExists(const YAML::Node &config, const std::string &field_name,
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

template <typename T>
auto ParseEnvVarIfExist(const std::string &field_name) -> tl::optional<T> {
  auto *env_var = std::getenv(field_name.c_str());
  if (env_var) {
    try {
      return ConvertStringToType<T>(env_var);
    } catch (std::invalid_argument e) {
      SPDLOG_ERROR("Could not convert env var: {} to type: {}", field_name,
                   typeid(T).name());
      return tl::nullopt;
    }
  }
  return tl::nullopt;
}

template <typename T>
auto UpdateVarIfEnvVarExists(T &var_to_update, const std::string &field_name)
    -> bool {
  auto var = ParseEnvVarIfExist<T>(field_name);
  if (var) {
    var_to_update = var.value();
    return true;
  }
  return false;
}

void ServerConfig::ParseYamlConfigFromString(const std::string &content) {
  YAML::Node config = YAML::Load(content);

  // ARBOR Variables
  number_of_nodes_arbor = ParseIfFieldExists<int>(
      config["arbor"], "numberOfNodes", number_of_nodes_arbor);

  base_url =
      ParseIfFieldExists<std::string>(config["nest"], "baseUrl", base_url);

  port_number_nodes_nest =
      ParseIfFieldExists<int>(config["nest"], "ports", port_number_nodes_nest);

  ports_consecutive_nest = ParseIfFieldExists<bool>(
      config["nest"], "portsConsecutive", ports_consecutive_nest);

  same_base_url_nest = ParseIfFieldExists<bool>(config["nest"], "sameBaseUrl",
                                                same_base_url_nest);

  // NEST Variables
  number_of_nodes_nest = ParseIfFieldExists<int>(
      config["nest"], "numberOfNodes", number_of_nodes_nest);

  base_url =
      ParseIfFieldExists<std::string>(config["nest"], "baseUrl", base_url);

  port_number_nodes_nest =
      ParseIfFieldExists<int>(config["nest"], "ports", port_number_nodes_nest);

  ports_consecutive_nest = ParseIfFieldExists<bool>(
      config["nest"], "portsConsecutive", ports_consecutive_nest);

  same_base_url_nest = ParseIfFieldExists<bool>(config["nest"], "sameBaseUrl",
                                                same_base_url_nest);
  //
  // Access Node Variables
  port_number_access =
      ParseIfFieldExists<int>(config["accessNode"], "port", port_number_access);

  port_number_access_ws = ParseIfFieldExists<int>(
      config["accessNode"], "websocketPort", port_number_access_ws);
}

void ServerConfig::ParseYamlConfigFromFile(const std::string &filename) {
  std::ifstream fin(filename);
  ParseYamlConfigFromString(std::string(std::istreambuf_iterator<char>(fin),
                                        std::istreambuf_iterator<char>()));
}

void ServerConfig::ParseTomlConfigFromFile(const std::string &filename) {
  auto config = toml::parse_file(filename);
  std::stringstream yaml;
  yaml << toml::yaml_formatter(config);
  SPDLOG_DEBUG("[ServerConfig::ParseTomlConfigFromFile] Toml as Yaml:\n {}",
               yaml.str());
  ParseYamlConfigFromString(yaml.str());
}

void ServerConfig::ParseConfigFromEnv() {
  UpdateVarIfEnvVarExists(ports_consecutive_nest, "INSITE_PORTS_CONSECUTIVE");
  UpdateVarIfEnvVarExists(same_base_url_nest, "INSITE_SAME_BASE_URL");
  UpdateVarIfEnvVarExists(number_of_nodes_nest, "INSITE_NEST_NUM_NODES");
  UpdateVarIfEnvVarExists(base_url, "INSITE_NEST_BASE_URL");
  UpdateVarIfEnvVarExists(port_number_nodes_nest, "INSITE_NEST_PORT");
  UpdateVarIfEnvVarExists(port_number_access, "INSITE_ACCESS_NODE_PORT");
  UpdateVarIfEnvVarExists(port_number_access_ws, "INSITE_ACCESS_NODE_WSPORT");
}

void ServerConfig::GenerateUrls(std::vector<std::string> &url_container,
                                const std::string &base_url,
                                int port_number_nodes) {
  std::string url("http://" + base_url + ":" +
                  std::to_string(port_number_nodes));
  url_container.push_back(url);
  SPDLOG_DEBUG(
      "[ServerConfig::GenerateRequestUrls] Added {} to the request urls", url);
}

void ServerConfig::GenerateRequestUrls() {
  if (ports_consecutive_nest && same_base_url_nest) {
    for (int i = 0; i < number_of_nodes_nest; ++i) {
      GenerateUrls(request_nest_urls, base_url, port_number_nodes_nest + i);
    }
  }

  if (ports_consecutive_arbor && same_base_url_arbor) {
    for (int i = 0; i < number_of_nodes_arbor; ++i) {
      GenerateUrls(request_arbor_urls, base_url_arbor,
                   port_number_nodes_arbor + i);
    }
  }
}

std::string ServerConfig::ToString() const {
  return fmt::format("Server Config:\n"
                     "Simulation Nodes NEST #: {}\n"
                     "Simulation Nodes ARBOR #: {}\n"
                     "Simulation Nodes Ports: {}\n"
                     "Simulation Nodes URLs: {}\n"
                     "Simulation Nodes Same Base URL: {}\n"
                     "Simulation Nodes Ports Consecutive: {}\n"
                     "Access Node Port: {}\n",
                     "Access Node Websocket Port: {}\n", number_of_nodes_nest,
                     number_of_nodes_arbor, port_number_nodes_nest, base_url,
                     same_base_url_nest, ports_consecutive_nest,
                     port_number_access, port_number_access_ws);
}

void ServerConfig::ParseConfigIfExists() {
  if (std::filesystem::exists("config.yml")) {
    ParseYamlConfigFromFile("config.yml");
  } else if (std::filesystem::exists("config.yaml")) {
    ParseYamlConfigFromFile("config.yaml");
  } else if (std::filesystem::exists("config.toml")) {
    ParseTomlConfigFromFile("config.toml");
  } else {
    ParseConfigFromEnv();
  }

  SPDLOG_DEBUG("{}", ToString());
}

} // namespace insite

template <> struct fmt::formatter<insite::ServerConfig> {
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const insite::ServerConfig &config, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // clang-format off
    return fmt::format_to(ctx.out(),config.ToString());
    // clang-format on
  }
};
