#include "host.hpp"

#include "packager.hpp"
#include "vre/backend.hpp"
#include "vre/config.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace studio {

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

AssetStats from_packed(const fs::path& self, const std::vector<PakEntry>& entries) {
    AssetStats s;
    s.mode = "packed";
    std::vector<char> data;
    for (const auto& e : entries) {
        if (!read_pak_data(self, e, data)) continue;  // a damaged entry is not counted
        ++s.files;
        s.bytes += data.size();
    }
    return s;
}

AssetStats from_folder(const fs::path& dir, const fs::path& self) {
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

AssetStats discover_assets(const fs::path& self) {
    std::vector<PakEntry> index;
    if (read_pak_index(self, index) && !index.empty()) return from_packed(self, index);
    return from_folder(self.parent_path(), self);
}

int run_host(int argc, char** argv, const char* title, const std::function<void(std::uint32_t)>& frame_step) {
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

    const AssetStats assets = discover_assets(self_path());
    vre::Backend* backend = vre::select_backend("");

    std::printf("%s\n", title);
    std::printf("assets: %s, %zu files, %llu bytes\n", assets.mode.c_str(), assets.files,
                static_cast<unsigned long long>(assets.bytes));
    std::printf("eye mode: %s, curvature: %s, backend: %s\n", vre::to_string(vre::build_eye_mode()),
                vre::to_string(vre::default_curvature(vre::build_eye_mode())), backend->name());

    std::uint32_t frames_run = 0;
    for (std::uint32_t f = 0; f < frames; ++f) {
        if (!backend->begin_frame()) break;
        if (frame_step) frame_step(f);
        backend->end_frame();
        ++frames_run;
    }
    backend->shutdown();
    std::printf("frames: %u of %u\n", frames_run, frames);

    if (expect_assets >= 0 && assets.files != static_cast<std::size_t>(expect_assets)) {
        std::fprintf(stderr, "expected %lld assets, found %zu\n", expect_assets, assets.files);
        return 1;
    }
    return frames_run == frames ? 0 : 1;
}

}  // namespace studio
