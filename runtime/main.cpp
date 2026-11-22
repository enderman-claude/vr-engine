// vre_runtime: the generic game host. Packaged games use the same host (see studio/host.hpp).
#include "host.hpp"

int main(int argc, char** argv) {
    return studio::run_host(argc, argv, "vre_runtime", nullptr);
}
