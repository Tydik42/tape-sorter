#include "tape_config.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <stdexcept>

namespace {
constexpr std::array<char const*, 4> kValidKeys = {"read_delay", "write_delay", "rewind_delay",
                                                   "move_delay"};
}  // namespace

TapeDelays ConfigParser::Parse(std::string const& config_path) {
    std::ifstream file(config_path);
    if (!file) throw std::runtime_error("Config file not found: " + config_path);

    TapeDelays delays;
    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;
        std::erase_if(line, ::isspace);
        if (line.empty()) continue;

        auto const delimiter_pos = line.find('=');
        if (delimiter_pos == std::string::npos) continue;

        auto const key = line.substr(0, delimiter_pos);
        auto const value = line.substr(delimiter_pos + 1);

        if (!IsValidKey(key)) {
            throw std::runtime_error("Invalid key: " + key);
        }

        try {
            auto ms_value = std::chrono::milliseconds(std::stoi(value));
            SetDelay(delays, key, ms_value);
        } catch (...) {
            throw std::runtime_error("Invalid value for key: " + key + " on line " +
                                     std::to_string(line_num));
        }
    }

    return delays;
}

constexpr bool ConfigParser::IsValidKey(std::string const& key) noexcept {
    return std::ranges::any_of(kValidKeys, [&key](char const* valid) { return key == valid; });
}

void ConfigParser::SetDelay(TapeDelays& delays, std::string const& key,
                            std::chrono::milliseconds const& value) {
    if (key == "read_delay") delays.read_delay_ms_ = value;
    if (key == "write_delay") delays.write_delay_ms_ = value;
    if (key == "rewind_delay") delays.rewind_delay_ms_ = value;
    if (key == "move_delay") delays.move_delay_ms_ = value;
}
