#pragma once
#include <cstddef>
#include <filesystem>
#include <cstdint>
#include <string>
#include <vector>

namespace studio {

enum class PackageKind { SingleFile, Archive };

struct PackageResult {
    bool ok = false;
    std::size_t files = 0;
    std::string error;
};

// Packages a project directory with a runtime executable.
//  SingleFile: one executable = runtime bytes + an appended asset table.
//  Archive:    a folder with the runtime and the project's folder tree.
PackageResult package_project(const std::filesystem::path& project_dir,
                              const std::filesystem::path& runtime,
                              const std::filesystem::path& out,
                              PackageKind kind);

struct PakEntry {
    std::string rel;           // relative path, '/' separators
    std::uint64_t offset = 0;  // where the entry's data starts in the file
    std::uint64_t size = 0;
};

// Reads the asset table appended to a file (single-file packages, or a runtime carrying one).
// Returns false if the file has no valid table.
bool read_pak_index(const std::filesystem::path& file, std::vector<PakEntry>& entries);
bool read_pak_data(const std::filesystem::path& file, const PakEntry& entry, std::vector<char>& data);

// Checks that a single-file package has a valid asset table; returns its entry count.
bool verify_single_file(const std::filesystem::path& file, std::size_t& entries);

}  // namespace studio
