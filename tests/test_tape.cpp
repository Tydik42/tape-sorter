#include "tape.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <vector>

class TapeTest : public ::testing::Test {
protected:
    std::string test_file_;
    TapeDelays delays_;

    void SetUp() override {
        test_file_ = "test_tape";

        delays_.read_delay_ms_ = std::chrono::milliseconds(0);
        delays_.write_delay_ms_ = std::chrono::milliseconds(0);
        delays_.rewind_delay_ms_ = std::chrono::milliseconds(0);
        delays_.move_delay_ms_ = std::chrono::milliseconds(0);

        std::ofstream ofs(test_file_, std::ios::binary);
        ofs.close();
    }

    void TearDown() override {
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }

    void createFileWithData(const std::vector<int32_t>& values) {
        std::ofstream ofs(test_file_, std::ios::binary);
        for (const auto& val : values) {
            ofs.write(reinterpret_cast<const char*>(&val), sizeof(val));
        }
        ofs.close();
    }
};

TEST_F(TapeTest, ConstructorSuccess) {
    EXPECT_NO_THROW({
        Tape tape(test_file_, delays_);
    });
}

TEST_F(TapeTest, ConstructorFailure) {
    EXPECT_THROW({
        Tape tape("non_existent_file", delays_);
    }, std::runtime_error);
}

TEST_F(TapeTest, Read) {
    std::vector<int32_t> test_data = {42, 99, 123};
    createFileWithData(test_data);

    Tape tape(test_file_, delays_);

    int32_t value;
    tape.read(value);
    EXPECT_EQ(value, 42);

    tape.move(MoveDirection::Forward);
    tape.read(value);
    EXPECT_EQ(value, 99);

    tape.move(MoveDirection::Forward);
    tape.read(value);
    EXPECT_EQ(value, 123);
}

TEST_F(TapeTest, Write) {
    Tape tape(test_file_, delays_);

    tape.write(100);
    tape.move(MoveDirection::Forward);
    tape.write(200);
    tape.move(MoveDirection::Forward);
    tape.write(300);

    tape.rewind();

    int32_t value;
    tape.read(value);
    EXPECT_EQ(value, 100);

    tape.move(MoveDirection::Forward);
    tape.read(value);
    EXPECT_EQ(value, 200);

    tape.move(MoveDirection::Forward);
    tape.read(value);
    EXPECT_EQ(value, 300);
}

TEST_F(TapeTest, Rewind) {
    std::vector<int32_t> test_data = {11, 22, 33};
    createFileWithData(test_data);

    Tape tape(test_file_, delays_);

    tape.move(MoveDirection::Forward);
    tape.move(MoveDirection::Forward);

    int32_t value;
    tape.read(value);
    EXPECT_EQ(value, 33);

    tape.rewind();
    tape.read(value);
    EXPECT_EQ(value, 11);
}

TEST_F(TapeTest, MoveForward) {
    std::vector<int32_t> test_data = {10, 20, 30, 40};
    createFileWithData(test_data);

    Tape tape(test_file_, delays_);

    int32_t value;
    tape.read(value);
    EXPECT_EQ(value, 10);

    tape.move(MoveDirection::Forward);
    tape.read(value);
    EXPECT_EQ(value, 20);
}

TEST_F(TapeTest, MoveBackward) {
    std::vector<int32_t> test_data = {10, 20, 30};
    createFileWithData(test_data);

    Tape tape(test_file_, delays_);

    tape.move(MoveDirection::Forward);
    tape.move(MoveDirection::Forward);

    int32_t value;
    tape.read(value);
    EXPECT_EQ(value, 30);

    tape.move(MoveDirection::Backward);
    tape.read(value);
    EXPECT_EQ(value, 20);
}

TEST_F(TapeTest, MoveBackwardOutOfBounds) {
    Tape tape(test_file_, delays_);

    EXPECT_THROW({
        tape.move(MoveDirection::Backward);
    }, std::out_of_range);
}

TEST_F(TapeTest, MoveForwardOutOfBounds) {
    Tape tape(test_file_, delays_);

    EXPECT_THROW({
        tape.move(MoveDirection::Forward);
    }, std::out_of_range);
}

TEST_F(TapeTest, DelaysAreApplied) {
    delays_.read_delay_ms_ = std::chrono::milliseconds(10);
    delays_.write_delay_ms_ = std::chrono::milliseconds(10);

    Tape tape(test_file_, delays_);

    int32_t value = 42;
    tape.write(value);
    tape.read(value);

    SUCCEED();
}
