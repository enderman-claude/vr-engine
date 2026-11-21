#pragma once
#include <cstdint>

namespace vre {

// Capability flags let the scheduler and render paths ask "can the runtime do X?"
// instead of checking the platform. Keep platform checks out of the core.
enum class Capability : std::uint32_t {
    None                     = 0,
    CylinderCompositionLayer = 1u << 0,
    EquirectCompositionLayer = 1u << 1,
    QuadCompositionLayer     = 1u << 2,
    Foveation                = 1u << 3,
    Multiview                = 1u << 4,
    HeadlessRender           = 1u << 5,
};

inline Capability operator|(Capability a, Capability b) {
    return static_cast<Capability>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

inline bool has(Capability set, Capability flag) {
    return (static_cast<std::uint32_t>(set) & static_cast<std::uint32_t>(flag)) != 0;
}

}  // namespace vre
