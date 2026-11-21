#include "studio.hpp"

#include "vre/backend.hpp"
#include "vre/job_system.hpp"

#include <atomic>

namespace studio {

namespace {
constexpr std::uint32_t kWorkItems = 4096;
}

RunResult run_project(const Project& project) {
    RunResult result;
    result.curvature = vre::default_curvature(project.eye_mode);

    vre::Backend* backend = vre::select_backend(project.backend);
    if (!backend) return result;
    result.backend_name = backend->name();

    const bool renders = project.eye_mode != vre::EyeMode::NoRender;
    vre::JobSystem jobs(project.job_threads);

    for (std::uint32_t f = 0; f < project.frames; ++f) {
        if (renders) {
            if (!backend->begin_frame()) break;
            std::atomic<std::uint64_t> sum{0};
            jobs.parallel_for(kWorkItems, [&sum](std::uint32_t begin, std::uint32_t end) {
                std::uint64_t local = 0;
                for (std::uint32_t i = begin; i < end; ++i) local += i;
                sum.fetch_add(local);
            });
            result.work_sum += sum.load();
            backend->end_frame();
        }
        ++result.frames_run;
    }

    backend->shutdown();
    return result;
}

}  // namespace studio
