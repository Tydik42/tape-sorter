#include "tape.h"

Tape::Tape(std::string const& file_name, TapeDelays const& delays)
    : tape_file_(file_name, std::fstream::in | std::fstream::out | std::fstream::binary),
      delays_(delays) {
    if (!tape_file_.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_name);
    }
    current_position_ = tape_file_.tellg();
}

void Tape::UpdatePosition() {
    tape_file_.seekg(current_position_);
    tape_file_.seekp(current_position_);
}

int32_t Tape::read(int32_t& value) {
    std::this_thread::sleep_for(delays_.read_delay_ms_);
    if (!tape_file_.is_open()) {
        throw std::runtime_error("File is not open");
    }

    tape_file_.seekg(current_position_);
    tape_file_.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (tape_file_.gcount() != sizeof(value)) {
        throw std::runtime_error("Failed to read from file");
    }

    UpdatePosition();
    return value;
}

void Tape::write(int32_t value) {
    std::this_thread::sleep_for(delays_.write_delay_ms_);
    if (!tape_file_.is_open()) {
        throw std::runtime_error("File is not open");
    }

    tape_file_.seekp(current_position_);
    tape_file_.write(reinterpret_cast<char*>(&value), sizeof(value));
    if (tape_file_.fail()) {
        throw std::runtime_error("Failed to write to file");
    }

    UpdatePosition();
}

void Tape::rewind() {
    std::this_thread::sleep_for(delays_.rewind_delay_ms_);
    tape_file_.clear();
    current_position_ = 0;
    UpdatePosition();
}

void Tape::move(MoveDirection direction) {
    std::this_thread::sleep_for(delays_.move_delay_ms_);
    std::streamoff const shift = (direction == MoveDirection::Forward)
                                         ? static_cast<std::streamoff>(sizeof(int32_t))
                                         : -static_cast<std::streamoff>(sizeof(int32_t));
    std::streampos const new_position = current_position_ + shift;

    if (new_position < 0) {
        throw std::out_of_range("New position is out of bounds");
    }

    tape_file_.seekg(0, std::ios::end);
    std::streampos const file_size = tape_file_.tellg();
    if (new_position > file_size) {
        throw std::out_of_range("New position is out of bounds");
    }

    current_position_ = new_position;
    UpdatePosition();
}
