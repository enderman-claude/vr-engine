#pragma once
#include <cstdint>
#include <functional>
#include <memory>

namespace vre {

// Job system wrapper (backed by enkiTS). Developers call parallel_for and never
// manage threads. Engine-internal: the dedicated per-eye/submission thread is
// not a job and is never scheduled through this class.
class JobSystem {
public:
    // threads == 0 selects the hardware default.
    explicit JobSystem(std::uint32_t threads = 0);
    ~JobSystem();

    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;

    // Calls body(begin, end) over disjoint ranges that together cover [0, count).
    // Blocks until all ranges complete.
    void parallel_for(std::uint32_t count,
                      const std::function<void(std::uint32_t begin, std::uint32_t end)>& body);

    std::uint32_t thread_count() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vre
