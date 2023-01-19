#include <chrono>
#include <utility>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"

namespace insite {
struct StopwatchHelper {
  std::vector<std::pair<std::chrono::duration<double>, std::string>> checkpoints;
  void checkpoint(const std::string& checkpoint_name) {
    checkpoints.push_back(std::make_pair(sw.elapsed(), checkpoint_name));
    sw.reset();
  }

  void reset() {
    sw.reset();
  }

  void print() {
    spdlog::info("Checkpoints: ");
    for (const auto& checkpoint : checkpoints) {
      spdlog::info("{} - {}", checkpoint.first, checkpoint.second);
    }
  }
  spdlog::stopwatch sw{};
};
}  // namespace insite
