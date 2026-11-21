#include "vre/backend.hpp"

#include <openvr.h>

namespace vre {

// SteamVR backend via the OpenVR SDK. Returns unavailable (so selection falls
// back) when no SteamVR runtime is installed or running, which is the case on CI.
class SteamVRBackend final : public Backend {
public:
    const char* name() const override { return "steamvr"; }

    // Left as None until each flag is verified against the OpenVR overlay API.
    Capability capabilities() const override { return Capability::None; }

    bool initialize() override {
        if (initialized_) return true;
        if (!vr::VR_IsRuntimeInstalled()) return false;
        vr::EVRInitError err = vr::VRInitError_None;
        vr::VR_Init(&err, vr::VRApplication_Scene);
        if (err != vr::VRInitError_None) return false;
        initialized_ = true;
        return true;
    }

    void shutdown() override {
        if (initialized_) {
            vr::VR_Shutdown();
            initialized_ = false;
        }
    }

    // Frame pacing (WaitGetPoses / submit) comes in M2. Only reports readiness now.
    bool begin_frame() override { return initialized_; }
    void end_frame() override {}

private:
    bool initialized_ = false;
};

Backend* make_steamvr_backend() {
    static SteamVRBackend instance;
    return &instance;
}

}  // namespace vre
