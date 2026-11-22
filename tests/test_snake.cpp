#include <doctest/doctest.h>

#include "snake.hpp"

TEST_CASE("snake moves one cell per step in its direction") {
    snake::Game g(10, 10, 1);
    const snake::Cell before = g.head();
    g.step();
    CHECK(g.head().x == before.x + 1);
    CHECK(g.head().y == before.y);
}

TEST_CASE("a reverse turn is ignored") {
    snake::Game g(10, 10, 1);
    const snake::Cell before = g.head();
    g.turn(snake::Dir::Left);  // moving right, so this is a reverse
    g.step();
    CHECK(g.head().x == before.x + 1);
}

TEST_CASE("hitting a wall ends the game") {
    snake::Game g(10, 10, 1);
    for (int i = 0; i < 10 && g.alive(); ++i) g.step();
    CHECK(!g.alive());
    CHECK(!g.step());
}

TEST_CASE("the autopilot eats food and keeps length equal to 3 plus score") {
    snake::Game g(20, 12, 2026);
    int steps = 0;
    while (g.alive() && steps < 400) {
        g.turn(g.autopilot());
        g.step();
        ++steps;
        CHECK(g.length() == 3 + static_cast<std::size_t>(g.score()));
    }
    CHECK(g.score() > 0);
}

TEST_CASE("the same seed plays the same game") {
    snake::Game a(20, 12, 7);
    snake::Game b(20, 12, 7);
    for (int i = 0; i < 50 && a.alive() && b.alive(); ++i) {
        a.turn(a.autopilot());
        a.step();
        b.turn(b.autopilot());
        b.step();
        CHECK(a.head() == b.head());
    }
}
