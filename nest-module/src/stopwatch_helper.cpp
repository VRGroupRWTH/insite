#include "stopwatch_helper.h"

namespace insite {

void StopwatchHelper::checkpoint(const std::string& checkpoint_name) {
  checkpoints.push_back(std::make_pair(sw.elapsed(), checkpoint_name));
  sw.reset();
}

void StopwatchHelper::reset() {
  sw.reset();
}

void StopwatchHelper::print() {
  SPDLOG_TRACE("Checkpoints: ");
  for (const auto& checkpoint : checkpoints) {
    SPDLOG_TRACE("{} - {}", checkpoint.first, checkpoint.second);
  }
}
}  // namespace insite
