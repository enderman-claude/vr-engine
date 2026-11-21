#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "packager.hpp"
#include "studio.hpp"
#include "vre/backend.hpp"
#include "vre/config.hpp"
#include "vre/job_system.hpp"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

void write_file(const fs::path& p, const std::string& text) {
    fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary);
    out << text;
}

// A project with a nested folder tree: 3 files across 3 folders.
fs::path make_project(const std::string& tag) {
    fs::path root = fs::temp_directory_path() / ("vre_test_project_" + tag);
    fs::remove_all(root);
    write_file(root / "assets" / "textures" / "wall.png", "PNGDATA");
    write_file(root / "assets" / "readme.txt", "hello");
    write_file(root / "scenes" / "main.scene", "scene");
    return root;
}

fs::path make_runtime(const std::string& tag) {
    fs::path rt = fs::temp_directory_path() / ("vre_test_runtime_" + tag + ".bin");
    write_file(rt, "RUNTIME");
    return rt;
}

}  // namespace

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

TEST_CASE("curvature is engine-managed: the VR curve is used only for stereo") {
    CHECK(vre::default_curvature(vre::EyeMode::Stereo) == vre::Curvature::Vr);
    CHECK(vre::default_curvature(vre::EyeMode::RightEyeToDesktop) == vre::Curvature::Flat);
    CHECK(vre::default_curvature(vre::EyeMode::LeftEyeToDesktop) == vre::Curvature::Flat);
    CHECK(vre::default_curvature(vre::EyeMode::Variable) == vre::Curvature::Flat);
    CHECK(vre::default_curvature(vre::EyeMode::NoVrRender) == vre::Curvature::Flat);
    CHECK(vre::default_curvature(vre::EyeMode::NoRender) == vre::Curvature::Flat);
}

TEST_CASE("eye mode names round-trip and unknown names are rejected") {
    const vre::EyeMode all[] = {vre::EyeMode::Stereo,           vre::EyeMode::RightEyeToDesktop,
                                vre::EyeMode::LeftEyeToDesktop, vre::EyeMode::Variable,
                                vre::EyeMode::NoVrRender,       vre::EyeMode::NoRender};
    for (vre::EyeMode m : all) {
        vre::EyeMode parsed = vre::EyeMode::Variable;
        CHECK(vre::parse_eye_mode(vre::to_string(m), parsed));
        CHECK(parsed == m);
    }
    vre::EyeMode untouched = vre::EyeMode::NoRender;
    CHECK(!vre::parse_eye_mode("bogus", untouched));
    CHECK(untouched == vre::EyeMode::NoRender);
}

TEST_CASE("studio runs the requested frames through the null backend") {
    studio::Project p;
    p.backend = "null";
    p.eye_mode = vre::EyeMode::Stereo;
    p.frames = 5;
    p.job_threads = 2;

    const studio::RunResult r = studio::run_project(p);
    CHECK(r.frames_run == 5);
    CHECK(r.backend_name == "null");
    CHECK(r.curvature == vre::Curvature::Vr);
    // Each frame sums 0..4095 through the job system.
    CHECK(r.work_sum == 5ull * (4095ull * 4096ull / 2ull));
}

TEST_CASE("studio falls back cleanly when SteamVR is requested without a runtime") {
    studio::Project p;
    p.backend = "steamvr";
    p.frames = 2;
    const studio::RunResult r = studio::run_project(p);
    CHECK((r.backend_name == "null" || r.backend_name == "steamvr"));
    CHECK(r.frames_run == 2);
}

TEST_CASE("no_render mode counts frames but does no work") {
    studio::Project p;
    p.backend = "null";
    p.eye_mode = vre::EyeMode::NoRender;
    p.frames = 5;
    const studio::RunResult r = studio::run_project(p);
    CHECK(r.frames_run == 5);
    CHECK(r.work_sum == 0);
}

TEST_CASE("archive packaging keeps the folder tree and writes a manifest") {
    const fs::path project = make_project("archive");
    const fs::path runtime = make_runtime("archive");
    const fs::path out = fs::temp_directory_path() / "vre_test_out_archive";
    fs::remove_all(out);

    const auto res = studio::package_project(project, runtime, out, studio::PackageKind::Archive);
    CHECK(res.ok);
    CHECK(res.files == 3);
    CHECK(fs::exists(out / runtime.filename()));
    CHECK(fs::exists(out / "assets" / "textures" / "wall.png"));
    CHECK(fs::exists(out / "assets" / "readme.txt"));
    CHECK(fs::exists(out / "scenes" / "main.scene"));
    CHECK(fs::exists(out / "manifest.txt"));
}

TEST_CASE("single-file packaging appends a verifiable asset table to the runtime") {
    const fs::path project = make_project("single");
    const fs::path runtime = make_runtime("single");
    const fs::path out = fs::temp_directory_path() / "vre_test_game_single.bin";
    fs::remove(out);

    const auto res = studio::package_project(project, runtime, out, studio::PackageKind::SingleFile);
    CHECK(res.ok);
    CHECK(res.files == 3);

    std::size_t entries = 0;
    CHECK(studio::verify_single_file(out, entries));
    CHECK(entries == 3);

    // The runtime bytes come first, unchanged.
    std::ifstream in(out, std::ios::binary);
    std::string head(7, '\0');
    in.read(head.data(), 7);
    CHECK(head == "RUNTIME");
}

TEST_CASE("packaging reports a clear error for a missing project") {
    const auto res = studio::package_project(fs::temp_directory_path() / "vre_no_such_project",
                                             make_runtime("missing"),
                                             fs::temp_directory_path() / "vre_test_out_missing",
                                             studio::PackageKind::Archive);
    CHECK(!res.ok);
    CHECK(!res.error.empty());
}
