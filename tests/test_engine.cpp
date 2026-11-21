#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "vre/backend.hpp"
#include "vre/job_system.hpp"

#include <atomic>
#include <glm/glm.hpp>
#include <vector>

TEST_CASE("null backend is always selectable as the fallback") {
    vre::Backend* b = vre::select_backend("does-not-exist");
    REQUIRE(b != nullptr);
    CHECK(std::string(b->name()) == "null");
    CHECK(vre::has(b->capabilities(), vre::Capability::HeadlessRender));
    CHECK(b->begin_frame());
    b->end_frame();
    b->shutdown();
}

TEST_CASE("capability flags compose and query correctly") {
    auto caps = vre::Capability::CylinderCompositionLayer | vre::Capability::Foveation;
    CHECK(vre::has(caps, vre::Capability::Foveation));
    CHECK(!vre::has(caps, vre::Capability::Multiview));
}

TEST_CASE("job system parallel_for visits every index exactly once") {
    vre::JobSystem jobs;
    CHECK(jobs.thread_count() >= 1);

    const std::uint32_t n = 10000;
    std::vector<std::atomic<int>> hits(n);
    jobs.parallel_for(n, [&](std::uint32_t begin, std::uint32_t end) {
        for (std::uint32_t i = begin; i < end; ++i) hits[i].fetch_add(1);
    });
    for (auto& h : hits) CHECK(h.load() == 1);
}

TEST_CASE("glm is linked and computes") {
    glm::vec3 v(3.0f, 4.0f, 0.0f);
    CHECK(glm::length(v) == doctest::Approx(5.0f));
}
