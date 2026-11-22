#include "packager.hpp"
#include "studio.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <string>

namespace fs = std::filesystem;

namespace {

void usage() {
    std::puts("usage:\n"
              "  vre_studio [--eye-mode M] [--backend B] [--frames N] [--threads N]\n"
              "  vre_studio package --project-dir D --kind single|archive --out O [--runtime R]\n"
              "    (R defaults to vre_runtime next to this executable)");
}

// Parses "--key value" pairs starting at argv[start].
bool parse_flags(int argc, char** argv, int start, std::map<std::string, std::string>& out) {
    for (int i = start; i < argc; i += 2) {
        const std::string key = argv[i];
        if (key.rfind("--", 0) != 0 || i + 1 >= argc) return false;
        out[key.substr(2)] = argv[i + 1];
    }
    return true;
}

fs::path default_runtime(const char* argv0) {
    fs::path name = "vre_runtime";
#ifdef _WIN32
    name += ".exe";
#endif
    return fs::path(argv0).parent_path() / name;
}

int run_package(const std::map<std::string, std::string>& flags, const char* argv0) {
    auto get = [&flags](const char* key) -> std::string {
        auto it = flags.find(key);
        return it == flags.end() ? std::string() : it->second;
    };
    const std::string kind = get("kind");
    if (get("project-dir").empty() || get("out").empty() || (kind != "single" && kind != "archive")) {
        usage();
        return 2;
    }
    const fs::path runtime = get("runtime").empty() ? default_runtime(argv0) : fs::path(get("runtime"));
    const auto pk = kind == "single" ? studio::PackageKind::SingleFile : studio::PackageKind::Archive;
    const auto result = studio::package_project(get("project-dir"), runtime, get("out"), pk);
    if (!result.ok) {
        std::fprintf(stderr, "package failed: %s\n", result.error.c_str());
        return 1;
    }
    std::printf("packaged %zu files as %s\n", result.files, kind.c_str());
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    const bool packaging = argc > 1 && std::string(argv[1]) == "package";
    std::map<std::string, std::string> flags;
    if (!parse_flags(argc, argv, packaging ? 2 : 1, flags)) {
        usage();
        return 2;
    }
    if (packaging) return run_package(flags, argv[0]);

    studio::Project project;
    if (flags.count("eye-mode") && !vre::parse_eye_mode(flags["eye-mode"], project.eye_mode)) {
        usage();
        return 2;
    }
    if (flags.count("backend")) project.backend = flags["backend"];
    if (flags.count("frames")) {
        project.frames = static_cast<std::uint32_t>(std::strtoul(flags["frames"].c_str(), nullptr, 10));
    }
    if (flags.count("threads")) {
        project.job_threads = static_cast<std::uint32_t>(std::strtoul(flags["threads"].c_str(), nullptr, 10));
    }

    const studio::RunResult r = studio::run_project(project);
    std::printf("eye mode: %s\ncurvature: %s\nbackend: %s\nframes: %u of %u\n", vre::to_string(project.eye_mode),
                vre::to_string(r.curvature), r.backend_name.c_str(), r.frames_run, project.frames);
    return r.frames_run == project.frames ? 0 : 1;
}
