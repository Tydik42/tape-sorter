#pragma once
#include <chrono>
#include <cstdint>

enum class MoveDirection { kForward, kBackward };

class ITape {
public:
    virtual ~ITape() = default;

    virtual bool Read(int32_t& value) = 0;
    virtual void Write(int32_t value) = 0;
    virtual void Move(MoveDirection direction) = 0;
    virtual void Rewind() = 0;
};
