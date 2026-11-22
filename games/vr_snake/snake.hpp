#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>

namespace snake {

enum class Dir { Up, Down, Left, Right };

struct Cell {
    int x = 0;
    int y = 0;
    bool operator==(const Cell& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Cell& o) const { return !(*this == o); }
};

Cell advance(Cell c, Dir d);

class Game {
public:
    Game(int width, int height, std::uint32_t seed);

    // Queues a turn for the next step. A reverse turn is ignored.
    void turn(Dir d);
    // Moves one cell. Returns false once the snake has died.
    bool step();
    void reset(std::uint32_t seed);

    // A safe direction toward the food, for the demo autopilot.
    Dir autopilot() const;

    bool alive() const { return alive_; }
    int score() const { return score_; }
    std::size_t length() const { return body_.size(); }
    Cell head() const { return body_.front(); }
    Cell food() const { return food_; }

private:
    bool in_bounds(Cell c) const;
    // allow_tail: the tail cell is free when the snake is not growing this step.
    bool hits_body(Cell c, bool allow_tail) const;
    void place_food();
    std::uint32_t next_random();

    int w_;
    int h_;
    std::deque<Cell> body_;  // front is the head
    Dir dir_ = Dir::Right;
    Dir pending_ = Dir::Right;
    Cell food_;
    int score_ = 0;
    bool alive_ = true;
    std::uint32_t rng_;
};

}  // namespace snake
