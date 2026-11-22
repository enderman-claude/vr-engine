// vre_runtime: the game host. A packaged game is this executable plus its assets.
// If an asset table is appended to this executable, it is a single-file build and
// the assets come from inside it. Otherwise assets are read from the folder it sits in.
#include "packager.hpp"
#include "studio.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace {

fs::path self_path() {
#ifdef _WIN32
    wchar_t buffer[32768];
    const DWORD n = GetModuleFileNameW(nullptr, buffer, static_cast<DWORD>(sizeof(buffer) / sizeof(buffer[0])));
    return fs::path(std::wstring(buffer, n));
#else
    return fs::read_symlink("/proc/self/exe");
#endif
}

struct AssetStats {
    std::string mode;
    std::size_t files = 0;
    std::uint64_t bytes = 0;
};

// Reads every packed asset, so a damaged table fails at startup.
AssetStats load_packed(const fs::path& self, const std::vector<studio::PakEntry>& entries) {
    AssetStats s;
    s.mode = "packed";
    std::vector<char> data;
    for (const auto& e : entries) {
        if (!studio::read_pak_data(self, e, data)) continue;
        ++s.files;
        s.bytes += data.size();
    }
    return s;
}

// Counts the files beside the runtime (archive build), skipping the runtime itself.
AssetStats load_folder(const fs::path& dir, const fs::path& self) {
    AssetStats s;
    s.mode = "folder";
    std::error_code ec;
    for (const auto& de : fs::recursive_directory_iterator(dir, ec)) {
        if (!de.is_regular_file() || de.path() == self) continue;
        ++s.files;
        s.bytes += static_cast<std::uint64_t>(fs::file_size(de.path(), ec));
    }
    return s;
}

}  // namespace

int main(int argc, char** argv) {
    long long expect_assets = -1;
    std::uint32_t frames = 60;
    for (int i = 1; i < argc; i += 2) {
        const std::string key = argv[i];
        if (i + 1 >= argc) {
            std::fprintf(stderr, "missing value for %s\n", key.c_str());
            return 2;
        }
        const std::string value = argv[i + 1];
        if (key == "--frames") {
            frames = static_cast<std::uint32_t>(std::strtoul(value.c_str(), nullptr, 10));
        } else if (key == "--expect-assets") {
            expect_assets = std::strtoll(value.c_str(), nullptr, 10);
        } else {
            std::fprintf(stderr, "unknown option %s\n", key.c_str());
            return 2;
        }
    }

    const fs::path self = self_path();
    std::vector<studio::PakEntry> index;
    const bool packed = studio::read_pak_index(self, index) && !index.empty();
    const AssetStats assets = packed ? load_packed(self, index) : load_folder(self.parent_path(), self);

    studio::Project project;
    project.frames = frames;
    const studio::RunResult r = studio::run_project(project);

    std::printf("assets: %s, %zu files, %llu bytes\n", assets.mode.c_str(), assets.files,
                static_cast<unsigned long long>(assets.bytes));
    std::printf("eye mode: %s, curvature: %s, backend: %s, frames: %u of %u\n", vre::to_string(project.eye_mode),
                vre::to_string(r.curvature), r.backend_name.c_str(), r.frames_run, frames);

    if (expect_assets >= 0 && assets.files != static_cast<std::size_t>(expect_assets)) {
        std::fprintf(stderr, "expected %lld assets, found %zu\n", expect_assets, assets.files);
        return 1;
    }
    return r.frames_run == frames ? 0 : 1;
}
