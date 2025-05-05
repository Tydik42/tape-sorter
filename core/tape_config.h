#pragma once

#include <chrono>
#include <string>

struct TapeDelays {
    std::chrono::milliseconds read_delay_ms_;
    std::chrono::milliseconds write_delay_ms_;
    std::chrono::milliseconds rewind_delay_ms_;
    std::chrono::milliseconds move_delay_ms_;

    constexpr explicit TapeDelays(std::chrono::milliseconds read = std::chrono::milliseconds(0),
                         std::chrono::milliseconds write = std::chrono::milliseconds(0),
                         std::chrono::milliseconds rewind = std::chrono::milliseconds(0),
                         std::chrono::milliseconds move = std::chrono::milliseconds(0))
        : read_delay_ms_(read),
          write_delay_ms_(write),
          rewind_delay_ms_(rewind),
          move_delay_ms_(move) {}
};

class ConfigParser {
public:
    static TapeDelays Parse(std::string const& config_path);

private:
    static constexpr bool IsValidKey(std::string const& key) noexcept;
    static void SetDelay(TapeDelays& delays, std::string const& key,
                         std::chrono::milliseconds const& value);
};
