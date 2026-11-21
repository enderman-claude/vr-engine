#include "vre/job_system.hpp"

#include "TaskScheduler.h"

namespace vre {

struct JobSystem::Impl {
    enki::TaskScheduler scheduler;
};

JobSystem::JobSystem(std::uint32_t threads) : impl_(new Impl) {
    if (threads == 0) {
        impl_->scheduler.Initialize();
    } else {
        impl_->scheduler.Initialize(threads);
    }
}

JobSystem::~JobSystem() = default;

void JobSystem::parallel_for(std::uint32_t count,
                             const std::function<void(std::uint32_t, std::uint32_t)>& body) {
    if (count == 0) return;
    enki::TaskSet task(count, [&body](enki::TaskSetPartition range, std::uint32_t) {
        body(range.start, range.end);
    });
    impl_->scheduler.AddTaskSetToPipe(&task);
    impl_->scheduler.WaitforTask(&task);
}

std::uint32_t JobSystem::thread_count() const {
    return impl_->scheduler.GetNumTaskThreads();
}

}  // namespace vre
