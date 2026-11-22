#include "snake.hpp"

#include <cstdlib>

namespace snake {

namespace {

bool opposite(Dir a, Dir b) {
    return (a == Dir::Up && b == Dir::Down) || (a == Dir::Down && b == Dir::Up) ||
           (a == Dir::Left && b == Dir::Right) || (a == Dir::Right && b == Dir::Left);
}

}  // namespace

Cell advance(Cell c, Dir d) {
    switch (d) {
        case Dir::Up: return {c.x, c.y - 1};
        case Dir::Down: return {c.x, c.y + 1};
        case Dir::Left: return {c.x - 1, c.y};
        case Dir::Right: return {c.x + 1, c.y};
    }
    return c;
}

Game::Game(int width, int height, std::uint32_t seed) : w_(width), h_(height), rng_(seed ? seed : 1u) {
    const Cell start{w_ / 2, h_ / 2};
    body_ = {start, {start.x - 1, start.y}, {start.x - 2, start.y}};
    place_food();
}

void Game::reset(std::uint32_t seed) {
    *this = Game(w_, h_, seed);
}

void Game::turn(Dir d) {
    if (!opposite(d, dir_)) pending_ = d;
}

bool Game::step() {
    if (!alive_) return false;
    dir_ = pending_;
    const Cell next = advance(body_.front(), dir_);
    const bool eating = next == food_;
    if (!in_bounds(next) || hits_body(next, !eating)) {
        alive_ = false;
        return false;
    }
    body_.push_front(next);
    if (eating) {
        ++score_;
        place_food();
    } else {
        body_.pop_back();
    }
    return true;
}

Dir Game::autopilot() const {
    Dir best = dir_;
    int best_dist = -1;
    const Dir all[] = {Dir::Up, Dir::Down, Dir::Left, Dir::Right};
    for (Dir d : all) {
        if (opposite(d, dir_)) continue;
        const Cell next = advance(head(), d);
        if (!in_bounds(next) || hits_body(next, next != food_)) continue;
        const int dist = std::abs(next.x - food_.x) + std::abs(next.y - food_.y);
        if (best_dist < 0 || dist < best_dist) {
            best = d;
            best_dist = dist;
        }
    }
    return best;  // no safe move left: keeps going and dies
}

bool Game::in_bounds(Cell c) const {
    return c.x >= 0 && c.x < w_ && c.y >= 0 && c.y < h_;
}

bool Game::hits_body(Cell c, bool allow_tail) const {
    const std::size_t n = allow_tail ? body_.size() - 1 : body_.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (body_[i] == c) return true;
    }
    return false;
}

void Game::place_food() {
    const auto w = static_cast<std::uint32_t>(w_);
    const auto h = static_cast<std::uint32_t>(h_);
    for (int attempt = 0; attempt < w_ * h_ * 4; ++attempt) {
        const Cell c{static_cast<int>(next_random() % w), static_cast<int>(next_random() % h)};
        if (!hits_body(c, false)) {
            food_ = c;
            return;
        }
    }
    for (int y = 0; y < h_; ++y) {
        for (int x = 0; x < w_; ++x) {
            const Cell c{x, y};
            if (!hits_body(c, false)) {
                food_ = c;
                return;
            }
        }
    }
    alive_ = false;  // the board is full
}

std::uint32_t Game::next_random() {
    rng_ = rng_ * 1664525u + 1013904223u;  // linear congruential generator: deterministic per seed
    return rng_ >> 8;
}

}  // namespace snake
