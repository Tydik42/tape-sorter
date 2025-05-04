#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "tape_config.h"

class ConfigParserTest : public ::testing::Test {
protected:
    std::string createTempConfigFile(const std::string& content) {
        std::string filename = "test_config_" + std::to_string(temp_files_.size()) + ".txt";
        std::ofstream file(filename);
        file << content;
        file.close();
        temp_files_.push_back(filename);
        return filename;
    }
    
    void TearDown() override {
        for (const auto& filename : temp_files_) {
            std::remove(filename.c_str());
        }
    }
    
private:
    std::vector<std::string> temp_files_;
};

TEST_F(ConfigParserTest, ParsesValidConfig) {
    std::string config = 
        "read_delay=100\n"
        "write_delay=200\n"
        "rewind_delay=300\n"
        "move_delay=400\n";
    
    auto filename = createTempConfigFile(config);
    auto delays = ConfigParser::parse(filename);
    
    EXPECT_EQ(delays.read_delay_ms_.count(), 100);
    EXPECT_EQ(delays.write_delay_ms_.count(), 200);
    EXPECT_EQ(delays.rewind_delay_ms_.count(), 300);
    EXPECT_EQ(delays.move_delay_ms_.count(), 400);
}

TEST_F(ConfigParserTest, ParsesPartialConfig) {
    std::string config = "read_delay=100\nwrite_delay=200\n";
    
    auto filename = createTempConfigFile(config);
    auto delays = ConfigParser::parse(filename);
    
    EXPECT_EQ(delays.read_delay_ms_.count(), 100);
    EXPECT_EQ(delays.write_delay_ms_.count(), 200);
    EXPECT_EQ(delays.rewind_delay_ms_.count(), 0);
    EXPECT_EQ(delays.move_delay_ms_.count(), 0);
}

TEST_F(ConfigParserTest, HandlesCommentsAndWhitespace) {
    std::string config = 
        "# This is a comment\n"
        "read_delay = 100 \n"
        "  # Another comment\n"
        "  write_delay=200  \n"
        "\n"
        "rewind_delay  =  300\n";
    
    auto filename = createTempConfigFile(config);
    auto delays = ConfigParser::parse(filename);
    
    EXPECT_EQ(delays.read_delay_ms_.count(), 100);
    EXPECT_EQ(delays.write_delay_ms_.count(), 200);
    EXPECT_EQ(delays.rewind_delay_ms_.count(), 300);
}

TEST_F(ConfigParserTest, ThrowsOnMissingFile) {
    EXPECT_THROW(ConfigParser::parse("nonexistent_file.txt"), std::runtime_error);
}

TEST_F(ConfigParserTest, ThrowsOnInvalidKey) {
    std::string config = "invalid_key=100\n";
    
    auto filename = createTempConfigFile(config);
    EXPECT_THROW(ConfigParser::parse(filename), std::runtime_error);
}

TEST_F(ConfigParserTest, ThrowsOnInvalidValue) {
    std::string config = "read_delay=abc\n";
    
    auto filename = createTempConfigFile(config);
    EXPECT_THROW(ConfigParser::parse(filename), std::runtime_error);

    config = "read_delay=-100\n";
    filename = createTempConfigFile(config);
}

TEST_F(ConfigParserTest, HandlesMultipleDefinitions) {
    std::string config = 
        "read_delay=100\n"
        "read_delay=200\n";
    
    auto filename = createTempConfigFile(config);
    auto delays = ConfigParser::parse(filename);

    EXPECT_EQ(delays.read_delay_ms_.count(), 200);
}

TEST_F(ConfigParserTest, IgnoresLinesWithoutDelimiter) {
    std::string config = 
        "read_delay=100\n"
        "this line has no delimiter\n"
        "write_delay=200\n";
    
    auto filename = createTempConfigFile(config);
    auto delays = ConfigParser::parse(filename);
    
    EXPECT_EQ(delays.read_delay_ms_.count(), 100);
    EXPECT_EQ(delays.write_delay_ms_.count(), 200);
}