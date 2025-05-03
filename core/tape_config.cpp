#include "tape_config.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <stdexcept>

namespace {
constexpr std::array<char const*, 4> kValidKeys = {"read_delay", "write_delay", "rewind_delay",
                                                   "shift_delay"};
}  // namespace

TapeDelays ConfigParser::parse(std::string const& config_path) {
    std::ifstream file(config_path);
    if (!file) throw std::runtime_error("Config file not found: " + config_path);

    TapeDelays delays;
    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line[0] == '#') continue;

        auto const delimiter_pos = line.find('=');
        if (delimiter_pos == std::string::npos) continue;

        auto const key = line.substr(0, delimiter_pos);
        auto const value = line.substr(delimiter_pos + 1);

        if (!isValidKey(key)) {
            throw std::runtime_error("Invalid key: " + key);
        }

        try {
            auto ms_value = std::chrono::milliseconds(std::stoi(value));
            setDelay(delays, key, ms_value);
        } catch (...) {
            throw std::runtime_error("Invalid value for key: " + key + " on line " +
                                     std::to_string(line_num));
        }
    }

    return delays;
}

constexpr bool ConfigParser::isValidKey(std::string const& key) noexcept {
    return std::any_of(kValidKeys.begin(), kValidKeys.end(),
                       [&key](char const* valid) { return key == valid; });
}

void ConfigParser::setDelay(TapeDelays& delays, std::string const& key,
                     std::chrono::milliseconds const& value) {
    if (key == "read_delay") delays.read_delay_ms_ = value;
    if (key == "write_delay") delays.write_delay_ms_ = value;
    if (key == "rewind_delay") delays.rewind_delay_ms_ = value;
    if (key == "shift_delay") delays.shift_delay_ms_ = value;
}
