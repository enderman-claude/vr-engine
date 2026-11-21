#include "vre/backend.hpp"

#include <vector>

namespace vre {

Backend* make_null_backend();
#if VRE_HAS_STEAMVR
Backend* make_steamvr_backend();
#endif

Backend* select_backend([[maybe_unused]] const std::string& preferred) {
    std::vector<Backend*> candidates;
#if VRE_HAS_STEAMVR
    if (preferred.empty() || preferred == "steamvr") candidates.push_back(make_steamvr_backend());
#endif
    candidates.push_back(make_null_backend());

    for (Backend* b : candidates) {
        if (b->initialize()) return b;
    }
    return nullptr;
}

}  // namespace vre
