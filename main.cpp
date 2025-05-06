#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>

#include "tape.h"
#include "tape_sorter.h"
#include "tape_config.h"
#include "tmp_tape_factory.h"

void ConvertTextToBinary(const std::string& text_path, const std::string& binary_path) {
    std::ifstream input(text_path);
    if (!input) {
        throw std::runtime_error("Cannot open input text file: " + text_path);
    }

    std::ofstream output(binary_path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Cannot create binary file: " + binary_path);
    }

    int32_t value;
    while (input >> value) {
        output.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
}

void ConvertBinaryToText(const std::string& binary_path, const std::string& text_path) {
    std::ifstream input(binary_path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open binary file: " + binary_path);
    }

    std::ofstream output(text_path);
    if (!output) {
        throw std::runtime_error("Cannot create output text file: " + text_path);
    }

    int32_t value;
    while (input.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        output << value << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " <input_tape> <output_tape> <config_file> <block_size>" << std::endl;
            return 1;
        }

        std::string input_text_path = argv[1];
        std::string output_text_path = argv[2];
        std::string config_path = argv[3];
        size_t block_size = std::stoull(argv[4]);

        TapeDelays delays = ConfigParser::Parse(config_path);

        std::string input_bin_path = input_text_path + ".bin";
        std::string output_bin_path = output_text_path + ".bin";

        std::cout << "Converting input to binary format..." << std::endl;
        ConvertTextToBinary(input_text_path, input_bin_path);

        {
            std::ofstream output_file(output_bin_path, std::ios::binary);
            if (!output_file) {
                throw std::runtime_error("Cannot create output binary file");
            }
        }

        std::cout << "Creating tapes..." << std::endl;
        Tape input_tape(input_bin_path, delays);
        Tape output_tape(output_bin_path, delays);

        std::string temp_dir = std::filesystem::temp_directory_path().string();
        auto factory = std::make_unique<TmpTapeFactory>(temp_dir, delays);

        std::cout << "Sorting with block size: " << block_size << "..." << std::endl;
        auto start_time = std::chrono::high_resolution_clock::now();

        TapeSorter sorter(block_size, std::move(factory));
        sorter.Sort(input_tape, output_tape);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Converting output to text format..." << std::endl;
        ConvertBinaryToText(output_bin_path, output_text_path);

        std::filesystem::remove(input_bin_path);
        std::filesystem::remove(output_bin_path);

        std::cout << "Sorting completed in " << duration.count() << " ms" << std::endl;

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
