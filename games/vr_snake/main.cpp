// VR Snake: the official test game.
// This build plays the game with an autopilot and runs it through the engine host.
// Headset rendering is not built yet; see NOTES.md.
#include "host.hpp"
#include "snake.hpp"

#include <cstdio>

int main(int argc, char** argv) {
    constexpr std::uint32_t kSeed = 2026u;
    snake::Game game(20, 12, kSeed);
    std::uint32_t deaths = 0;

    const int rc = studio::run_host(
        argc, argv, "VR Snake (autopilot, desktop build; headset rendering not built yet)",
        [&game, &deaths](std::uint32_t frame) {
            if (!game.alive()) {
                ++deaths;
                game.reset(kSeed + frame);
            }
            game.turn(game.autopilot());
            game.step();
        });

    std::printf("score: %d, length: %zu, deaths: %u\n", game.score(), game.length(), deaths);
    return rc;
}
