#pragma once

#include <chrono>
#include <string>

struct TapeDelays {
    std::chrono::milliseconds read_delay_ms_;
    std::chrono::milliseconds write_delay_ms_;
    std::chrono::milliseconds rewind_delay_ms_;
    std::chrono::milliseconds shift_delay_ms_;

    constexpr TapeDelays(std::chrono::milliseconds read = std::chrono::milliseconds(0),
                         std::chrono::milliseconds write = std::chrono::milliseconds(0),
                         std::chrono::milliseconds rewind = std::chrono::milliseconds(0),
                         std::chrono::milliseconds shift = std::chrono::milliseconds(0))
        : read_delay_ms_(read),
          write_delay_ms_(write),
          rewind_delay_ms_(rewind),
          shift_delay_ms_(shift) {}
};

class ConfigParser {
public:
    static TapeDelays parse(std::string const& config_path);

private:
    static constexpr bool isValidKey(std::string const& key) noexcept;
    static void setDelay(TapeDelays& delays, std::string const& key,
                         std::chrono::milliseconds const& value);
};
