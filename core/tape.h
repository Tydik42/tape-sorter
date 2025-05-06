#pragma once
#include <fstream>
#include <thread>
#include <vector>

#include "i_tape.h"
#include "tape_config.h"

class Tape : public ITape {
private:
    std::fstream tape_file_;
    std::streampos current_position_ = 0;
    TapeDelays delays_;

    void UpdatePosition();
    std::streampos GetFileSize();

public:
    Tape(std::string const& file_name, TapeDelays const& delays);

    bool Read(int32_t& value) override;
    void Write(int32_t value) override;
    void Move(MoveDirection direction) override;
    void Rewind() override;
};
