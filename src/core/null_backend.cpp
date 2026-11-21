#include "vre/backend.hpp"

namespace vre {

// Headless backend: no display, no runtime. Used for tests, tools, and servers.
class NullBackend final : public Backend {
public:
    const char* name() const override { return "null"; }
    Capability capabilities() const override { return Capability::HeadlessRender; }
    bool initialize() override { return true; }
    void shutdown() override {}
    bool begin_frame() override { return true; }
    void end_frame() override {}
};

Backend* make_null_backend() {
    static NullBackend instance;
    return &instance;
}

}  // namespace vre
