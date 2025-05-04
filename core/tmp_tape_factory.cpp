#include "tmp_tape_factory.h"

#include <filesystem>
#include <iostream>

#include "tape.h"

TmpTapeFactory::TmpTapeFactory(std::string const &dir_name, TapeDelays const &delays)
    : dir_name_(dir_name), delays_(delays) {
    std::filesystem::create_directories(dir_name_);
}

std::unique_ptr<ITape> TmpTapeFactory::Create() {
    std::string tape_name = GenerateTapeName();
    std::ofstream file(tape_name);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create file: " + tape_name);
    }
    file.close();

    created_tapes_.push_back(tape_name);
    return std::make_unique<Tape>(tape_name, delays_);
}

void TmpTapeFactory::CleanupTempFiles() const {
    for (auto const &tape_name : created_tapes_) {
        try {
            std::filesystem::remove(tape_name);
        } catch (std::filesystem::filesystem_error const &e) {
            std::cerr << "Error removing file: " << e.what() << std::endl;
        }
    }
}

std::string TmpTapeFactory::GenerateTapeName() const {
    static int tape_counter = 0;
    return dir_name_ + "/tmp_tape" + std::to_string(tape_counter++);
}

TmpTapeFactory::~TmpTapeFactory() {
    CleanupTempFiles();
}
