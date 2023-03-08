#pragma once
#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <chrono>
#include <utility>
#include <vector>

namespace insite {
struct StopwatchHelper {
  std::vector<std::pair<std::chrono::duration<double>, std::string>> checkpoints;

  void checkpoint(const std::string& checkpoint_name);
  void reset();
  void print();
  spdlog::stopwatch sw{};
};
}  // namespace insite
