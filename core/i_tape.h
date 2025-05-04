#pragma once
#include <chrono>
#include <cstdint>

enum class MoveDirection { Forward, Backward };

class ITape {
public:
    virtual ~ITape() = default;

    virtual int32_t read(int32_t& value) = 0;
    virtual void write(int32_t value) = 0;
    virtual void move(MoveDirection direction) = 0;
    virtual void rewind() = 0;
};
