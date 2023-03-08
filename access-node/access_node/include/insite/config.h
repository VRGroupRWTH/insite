#pragma once
#include <spdlog/fmt/fmt.h>
#include <string>
#include <vector>

namespace insite {

inline constexpr int kDefaultPortNumberAccessNode = 52056;
inline constexpr int kDefaultPortNumberSimulationNode = 18080;
inline constexpr int kDefaultPortNumberAccessWebsocket = 9010;

class ServerConfig {
 public:
  static ServerConfig& GetInstance() {
    static ServerConfig instance;  // Guaranteed to be destroyed.
                                   // Instantiated on first use.
    return instance;
  }

  ServerConfig() = default;  // Constructor? (the {} brackets) are needed here.

  void ParseTomlConfigFromFile(const std::string& filename);
  void ParseYamlConfigFromFile(const std::string& filename);
  void ParseYamlConfigFromString(const std::string& content);

  void ParseConfigIfExists();

  [[nodiscard]] std::string ToString() const;

  void GenerateRequestUrls();

  bool ports_consecutive = true;
  bool same_base_url = true;
  int number_of_nodes = 1;
  std::string base_url = "localhost";
  int port_number_nodes = kDefaultPortNumberSimulationNode;
  int port_number_access = kDefaultPortNumberAccessNode;
  int port_number_access_ws = kDefaultPortNumberAccessWebsocket;

  std::vector<std::string> request_urls;

  friend struct fmt::formatter<ServerConfig>;
};
}  // namespace insite
