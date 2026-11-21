#pragma once
#include "vre/capabilities.hpp"

#include <string>

namespace vre {

// Every platform/runtime implements this. The engine core talks only to this interface.
// Order of implementation: null -> SteamVR -> OpenXR (generic) -> Quest/Android.
class Backend {
public:
    virtual ~Backend() = default;

    virtual const char* name() const = 0;
    virtual Capability capabilities() const = 0;

    // Returns false if the runtime is unavailable; the caller may fall back.
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    virtual bool begin_frame() = 0;
    virtual void end_frame() = 0;
};

// Picks the first backend that initializes. Falls back to the null backend.
// Returns nullptr only if even the null backend fails (should never happen).
Backend* select_backend(const std::string& preferred);

}  // namespace vre
