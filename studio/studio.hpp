#pragma once
#include "vre/config.hpp"

#include <cstdint>
#include <string>

namespace studio {

struct Project {
    std::string name = "untitled";
    vre::EyeMode eye_mode = vre::build_eye_mode();
    std::string backend;  // empty = auto (SteamVR if available, else null)
    std::uint32_t frames = 60;
    std::uint32_t job_threads = 0;
};

struct RunResult {
    std::string backend_name = "none";
    vre::Curvature curvature = vre::Curvature::Flat;
    std::uint32_t frames_run = 0;
    std::uint64_t work_sum = 0;
};

// Runs the project's frame loop through the selected backend, with each frame's
// work dispatched through the job system.
RunResult run_project(const Project& project);

}  // namespace studio
