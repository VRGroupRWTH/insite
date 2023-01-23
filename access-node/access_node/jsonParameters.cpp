#include "jsonParameters.h"

namespace insite {

std::unordered_set<std::uint64_t> CommaListToSet(const char* input_string) {
  std::stringstream stringstream_in(input_string);
  std::unordered_set<std::uint64_t> output;
  std::uint64_t i = 0;
  while (stringstream_in >> i) {
    output.insert(i);
    stringstream_in.ignore(1);
  }
  return output;
}
}  // namespace insite
