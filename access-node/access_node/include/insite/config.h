#pragma once
#include <insite_defines.h>

#include <spdlog/fmt/fmt.h>
#include <string>
#include <vector>

namespace insite {

inline constexpr int kDefaultPortNumberAccessNode = 52056;
inline constexpr int kDefaultPortNumberSimulationNode = 18080;
inline constexpr int kDefaultPortNumberSimulationNodeArbor = 1337;
inline constexpr int kDefaultPortNumberAccessWebsocket = 9010;

class ServerConfig {
public:
  static ServerConfig &GetInstance() {
    static ServerConfig instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
    return instance;
  }

  ServerConfig() = default; // Constructor? (the {} brackets) are needed here.

  void ParseTomlConfigFromFile(const std::string &filename);
  void ParseYamlConfigFromFile(const std::string &filename);
  void ParseYamlConfigFromString(const std::string &content);
  void ParseConfigFromEnv();

  void ParseConfigIfExists();

  [[nodiscard]] std::string ToString() const;

  void GenerateRequestUrls();
  // void GenerateUrls();
  void GenerateUrls(std::string base_url, int port_number_nodes);

  void GenerateUrls(std::vector<std::string> &url_container,
                    const std::string &base_url, int port_number_nodes);

  bool ports_consecutive_nest = true;
  bool ports_consecutive_arbor = true;
  bool same_base_url_nest = true;
  bool same_base_url_arbor = true;
  int number_of_nodes_nest = 1;
  int number_of_nodes_arbor = 1;
  std::string base_url = "localhost";
  std::string base_url_arbor = "localhost";
  int port_number_nodes_nest = kDefaultPortNumberSimulationNode;
  int port_number_nodes_arbor = kDefaultPortNumberSimulationNodeArbor;
  int port_number_access = kDefaultPortNumberAccessNode;
  int port_number_access_ws = kDefaultPortNumberAccessWebsocket;

  std::vector<std::string> request_nest_urls;
  std::vector<std::string> request_arbor_urls;

  friend struct fmt::formatter<ServerConfig>;
};
} // namespace insite
