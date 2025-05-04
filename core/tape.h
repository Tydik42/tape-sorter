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
    std::vector<std::streampos> positions_;

    void updatePosition();
    void scanPositions();

    public : Tape(std::string const& file_path, TapeDelays const& delays);

    int32_t read(int32_t& value) override;
    void write(int32_t value) override;
    void move(MoveDirection direction) override;
    void rewind() override;
};
