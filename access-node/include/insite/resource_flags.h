#pragma once
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace insite {

enum class ResourceFlag : std::uint8_t {
  kNone = 1 << 0,
  kDebug = 1 << 1,
  kNest = 1 << 2,
  kTVB = 1 << 3,
};

inline ResourceFlag operator|(ResourceFlag lhs, ResourceFlag rhs) {
  return static_cast<ResourceFlag>(
      static_cast<std::underlying_type<ResourceFlag>::type>(lhs) |
      static_cast<std::underlying_type<ResourceFlag>::type>(rhs));
}

inline bool operator&(ResourceFlag lhs, ResourceFlag rhs) {
  return static_cast<bool>(
      static_cast<std::underlying_type<ResourceFlag>::type>(lhs) &
      static_cast<std::underlying_type<ResourceFlag>::type>(rhs));
}

struct WebsocketMessage {
  WebsocketMessage(std::string message, std::uint32_t receiver,
                   ResourceFlag flags)
      : message(std::move(message)), receiver(receiver), flags(flags) {}
  std::string message;
  std::uint32_t receiver;
  ResourceFlag flags;
};

// inline ResourceFlag operator&(ResourceFlag lhs, ResourceFlag rhs) {
//   return static_cast<ResourceFlag>(
//       static_cast<std::underlying_type<ResourceFlag>::type>(lhs) &
//       static_cast<std::underlying_type<ResourceFlag>::type>(rhs));
// }

} // namespace insite
