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

bool Tape::Read(int32_t& value) {
    std::this_thread::sleep_for(delays_.read_delay_ms_);
    if (!tape_file_.is_open()) {
        throw std::runtime_error("File is not open");
    }

    if (current_position_ >= GetFileSize()) {
        return false;
    }

    tape_file_.seekg(current_position_);
    tape_file_.read(reinterpret_cast<char*>(&value), sizeof(value));

    if (tape_file_.eof()) {
        tape_file_.clear();
        return false;
    }

    if (tape_file_.gcount() != sizeof(value)) {
        throw std::runtime_error("Failed to read from file");
    }
    return true;
}

void Tape::Write(int32_t value) {
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

void Tape::Rewind() {
    std::this_thread::sleep_for(delays_.rewind_delay_ms_);
    tape_file_.clear();
    current_position_ = 0;
    UpdatePosition();
}

void Tape::Move(MoveDirection direction) {
    std::this_thread::sleep_for(delays_.move_delay_ms_);
    std::streamoff const shift = (direction == MoveDirection::kForward)
                                         ? static_cast<std::streamoff>(sizeof(int32_t))
                                         : -static_cast<std::streamoff>(sizeof(int32_t));
    std::streampos const new_position = current_position_ + shift;

    if (new_position < 0) {
        throw std::out_of_range("New position is out of bounds");
    }

    current_position_ = new_position;
    UpdatePosition();
}

std::streampos Tape::GetFileSize() {
    tape_file_.seekg(0, std::ios::end);
    return tape_file_.tellg();
}
