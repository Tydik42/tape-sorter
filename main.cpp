#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "tape.h"
#include "tape_config.h"
#include "tape_sorter.h"
#include "tmp_tape_factory.h"

constexpr size_t kDefaultBlockSize = 32;

void ConvertTextToBinary(std::string const& text_path, std::string const& binary_path) {
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
        output.write(reinterpret_cast<char const*>(&value), sizeof(value));
    }
}

void ConvertBinaryToText(std::string const& binary_path, std::string const& text_path) {
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

void PrintHelp() {
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                Show this help message" << std::endl;
    std::cout << "  -i, --input FILE          Input tape file (required)" << std::endl;
    std::cout << "  -o, --output FILE         Output tape file (required)" << std::endl;
    std::cout << "  -c, --config FILE         Configuration file (default is 0 on all operations)"
              << std::endl;
    std::cout << "  -b, --block-size SIZE     Memory block size (default: " << kDefaultBlockSize
              << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file format:" << std::endl;
    std::cout << "  read_delay=<milliseconds>" << std::endl;
    std::cout << "  write_delay=<milliseconds>" << std::endl;
    std::cout << "  rewind_delay=<milliseconds>" << std::endl;
    std::cout << "  move_delay=<milliseconds>" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        std::string input_text_path;
        std::string output_text_path;
        TapeDelays delays;
        size_t block_size = kDefaultBlockSize;

        if (argc == 1) {
            PrintHelp();
            return 0;
        }

        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") {
                PrintHelp();
                return 0;
            }
            if (arg == "-i" || arg == "--input") {
                if (i + 1 < argc) {
                    input_text_path = argv[++i];
                } else {
                    throw std::runtime_error("Missing input file path");
                }
            } else if (arg == "-o" || arg == "--output") {
                if (i + 1 < argc) {
                    output_text_path = argv[++i];
                } else {
                    throw std::runtime_error("Missing output file path");
                }
            } else if (arg == "-c" || arg == "--config") {
                if (i + 1 < argc) {
                    std::string config_path = argv[++i];
                    delays = ConfigParser::Parse(config_path);
                } else {
                    throw std::runtime_error("Missing config file path");
                }
            } else if (arg == "-b" || arg == "--block-size") {
                if (i + 1 < argc) {
                    block_size = std::stoull(argv[++i]);
                    if (block_size == 0) {
                        throw std::runtime_error("Block size must be greater than zero");
                    }
                } else {
                    throw std::runtime_error("Missing block size value");
                }
            } else {
                throw std::runtime_error("Unknown option: " + arg);
            }
        }

        if (input_text_path.empty()) {
            throw std::runtime_error("Input file path is required (use -i or --input)");
        }
        if (output_text_path.empty()) {
            throw std::runtime_error("Output file path is required (use -o or --output)");
        }

        std::string input_bin_path = input_text_path + ".bin";
        std::string output_bin_path = output_text_path + ".bin";

        ConvertTextToBinary(input_text_path, input_bin_path);

        {
            std::ofstream output_file(output_bin_path, std::ios::binary);
            if (!output_file) {
                throw std::runtime_error("Cannot create output binary file");
            }
        }

        Tape input_tape(input_bin_path, delays);
        Tape output_tape(output_bin_path, delays);

        std::string temp_dir = std::filesystem::temp_directory_path().string();
        auto factory = std::make_unique<TmpTapeFactory>(temp_dir, delays);

        TapeSorter sorter(block_size, std::move(factory));
        sorter.Sort(input_tape, output_tape);

        ConvertBinaryToText(output_bin_path, output_text_path);

        std::filesystem::remove(input_bin_path);
        std::filesystem::remove(output_bin_path);
        return 0;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
