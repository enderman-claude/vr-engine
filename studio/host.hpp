#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

namespace studio {

struct AssetStats {
    std::string mode;  // "packed" (inside this executable) or "folder" (beside it)
    std::size_t files = 0;
    std::uint64_t bytes = 0;
};

AssetStats discover_assets(const std::filesystem::path& self);

// Shared startup for every game executable. Parses --frames and --expect-assets, finds
// the packaged or folder assets, drives the engine backend for each frame, calls
// frame_step once per frame (may be empty), prints a summary, and returns the exit code.
int run_host(int argc, char** argv, const char* title, const std::function<void(std::uint32_t)>& frame_step);

}  // namespace studio
