#include "websocket_server.h"

namespace insite {

template <typename Key, typename Value, std::size_t Size>
struct Map {
  std::array<std::pair<Key, Value>, Size> data;
  constexpr Value At(const Key& key) const {
    const auto itr =
        std::find_if(begin(data), end(data),
                     [&key](const auto& value) { return value.first == key; });
    if (itr != end(data)) {
      return itr->second;
    }
    throw std::range_error("Not Found");
  }
};
}  // namespace insite