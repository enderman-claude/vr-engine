#include "vre/backend.hpp"

namespace vre {

// SteamVR backend. Milestone 2: real implementation via OpenVR/SteamVR APIs.
// For now it reports itself unavailable so selection falls back cleanly.
class SteamVRBackend final : public Backend {
public:
    const char* name() const override { return "steamvr"; }
    Capability capabilities() const override { return Capability::None; }
    bool initialize() override { return false; }  // TODO(M2): connect to SteamVR runtime
    void shutdown() override {}
    bool begin_frame() override { return false; }
    void end_frame() override {}
};

Backend* make_steamvr_backend();
Backend* make_steamvr_backend() {
    static SteamVRBackend instance;
    return &instance;
}

}  // namespace vre
