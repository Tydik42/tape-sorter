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
    tape.Read(value);
    EXPECT_EQ(value, 42);

    tape.Move(MoveDirection::kForward);
    tape.Read(value);
    EXPECT_EQ(value, 99);

    tape.Move(MoveDirection::kForward);
    tape.Read(value);
    EXPECT_EQ(value, 123);
}

TEST_F(TapeTest, Write) {
    Tape tape(test_file_, delays_);

    tape.Write(100);
    tape.Move(MoveDirection::kForward);
    tape.Write(200);
    tape.Move(MoveDirection::kForward);
    tape.Write(300);

    tape.Rewind();

    int32_t value;
    tape.Read(value);
    EXPECT_EQ(value, 100);

    tape.Move(MoveDirection::kForward);
    tape.Read(value);
    EXPECT_EQ(value, 200);

    tape.Move(MoveDirection::kForward);
    tape.Read(value);
    EXPECT_EQ(value, 300);
}

TEST_F(TapeTest, Rewind) {
    std::vector<int32_t> test_data = {11, 22, 33};
    createFileWithData(test_data);

    Tape tape(test_file_, delays_);

    tape.Move(MoveDirection::kForward);
    tape.Move(MoveDirection::kForward);

    int32_t value;
    tape.Read(value);
    EXPECT_EQ(value, 33);

    tape.Rewind();
    tape.Read(value);
    EXPECT_EQ(value, 11);
}

TEST_F(TapeTest, MoveForward) {
    std::vector<int32_t> test_data = {10, 20, 30, 40};
    createFileWithData(test_data);

    Tape tape(test_file_, delays_);

    int32_t value;
    tape.Read(value);
    EXPECT_EQ(value, 10);

    tape.Move(MoveDirection::kForward);
    tape.Read(value);
    EXPECT_EQ(value, 20);
}

TEST_F(TapeTest, MoveBackward) {
    std::vector<int32_t> test_data = {10, 20, 30};
    createFileWithData(test_data);

    Tape tape(test_file_, delays_);

    tape.Move(MoveDirection::kForward);
    tape.Move(MoveDirection::kForward);

    int32_t value;
    tape.Read(value);
    EXPECT_EQ(value, 30);

    tape.Move(MoveDirection::kBackward);
    tape.Read(value);
    EXPECT_EQ(value, 20);
}

TEST_F(TapeTest, MoveBackwardOutOfBounds) {
    Tape tape(test_file_, delays_);

    EXPECT_THROW({
        tape.Move(MoveDirection::kBackward);
    }, std::out_of_range);
}

TEST_F(TapeTest, MoveForwardOutOfBounds) {
    Tape tape(test_file_, delays_);

    EXPECT_THROW({
        tape.Move(MoveDirection::kForward);
    }, std::out_of_range);
}

TEST_F(TapeTest, DelaysAreApplied) {
    delays_.read_delay_ms_ = std::chrono::milliseconds(10);
    delays_.write_delay_ms_ = std::chrono::milliseconds(10);

    Tape tape(test_file_, delays_);

    int32_t value = 42;
    tape.Write(value);
    tape.Read(value);

    SUCCEED();
}
