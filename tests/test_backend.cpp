#include "vre/backend.hpp"

#include <cstdio>
#include <cstdlib>

static int failures = 0;

#define CHECK(cond)                                                      \
    do {                                                                 \
        if (!(cond)) {                                                   \
            std::fprintf(stderr, "FAILED %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++failures;                                                  \
        }                                                                \
    } while (0)

int main() {
    // Null backend must always be selectable (headless fallback).
    vre::Backend* b = vre::select_backend("does-not-exist");
    CHECK(b != nullptr);
    if (b) {
        CHECK(std::string(b->name()) == "null");
        CHECK(vre::has(b->capabilities(), vre::Capability::HeadlessRender));
        CHECK(b->begin_frame());
        b->end_frame();
        b->shutdown();
    }

    // Capability flags compose and query correctly.
    auto caps = vre::Capability::CylinderCompositionLayer | vre::Capability::Foveation;
    CHECK(vre::has(caps, vre::Capability::Foveation));
    CHECK(!vre::has(caps, vre::Capability::Multiview));

    if (failures == 0) std::puts("all backend tests passed");
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
