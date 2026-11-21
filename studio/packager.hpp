#pragma once
#include <cstddef>
#include <filesystem>
#include <string>

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

// Checks that a single-file package has a valid asset table; returns its entry count.
bool verify_single_file(const std::filesystem::path& file, std::size_t& entries);

}  // namespace studio
