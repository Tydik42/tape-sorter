#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

#include "tape.h"

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

    void CreateFileWithData(std::vector<int32_t> const& values) const {
        std::ofstream ofs(test_file_, std::ios::binary);
        for (auto const& val : values) {
            ofs.write(reinterpret_cast<char const*>(&val), sizeof(val));
        }
        ofs.close();
    }
};

TEST_F(TapeTest, ConstructorSuccess) {
    EXPECT_NO_THROW({ Tape tape(test_file_, delays_); });
}

TEST_F(TapeTest, ConstructorFailure) {
    EXPECT_THROW({ Tape tape("non_existent_file", delays_); }, std::runtime_error);
}

TEST_F(TapeTest, Read) {
    std::vector<int32_t> test_data = {42, 99, 123};
    CreateFileWithData(test_data);

    Tape tape(test_file_, delays_);

    int32_t value;
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 42);

    tape.Move(MoveDirection::kForward);
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 99);

    tape.Move(MoveDirection::kForward);
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 123);
}

TEST_F(TapeTest, ReadAtEOF) {
    std::vector<int32_t> test_data = {42};
    CreateFileWithData(test_data);

    Tape tape(test_file_, delays_);

    tape.Move(MoveDirection::kForward);
    int32_t value;
    EXPECT_FALSE(tape.Read(value));
}

TEST_F(TapeTest, Write) {
    Tape tape(test_file_, delays_);

    tape.Write(100);
    tape.Write(200);

    tape.Rewind();
    int32_t value;
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 200);

    tape.Rewind();
    tape.Write(100);
    tape.Move(MoveDirection::kForward);
    tape.Write(200);
    tape.Move(MoveDirection::kForward);
    tape.Write(300);

    tape.Rewind();
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 100);

    tape.Move(MoveDirection::kForward);
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 200);

    tape.Move(MoveDirection::kForward);
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 300);
}

TEST_F(TapeTest, Rewind) {
    std::vector<int32_t> test_data = {11, 22, 33};
    CreateFileWithData(test_data);

    Tape tape(test_file_, delays_);

    tape.Move(MoveDirection::kForward);
    tape.Move(MoveDirection::kForward);

    int32_t value;
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 33);

    tape.Rewind();
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 11);
}

TEST_F(TapeTest, MoveForward) {
    std::vector<int32_t> test_data = {10, 20, 30, 40};
    CreateFileWithData(test_data);

    Tape tape(test_file_, delays_);

    int32_t value;
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 10);

    tape.Move(MoveDirection::kForward);
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 20);
}

TEST_F(TapeTest, MoveBackward) {
    std::vector<int32_t> test_data = {10, 20, 30};
    CreateFileWithData(test_data);

    Tape tape(test_file_, delays_);

    tape.Move(MoveDirection::kForward);
    tape.Move(MoveDirection::kForward);

    int32_t value;
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 30);

    tape.Move(MoveDirection::kBackward);
    EXPECT_TRUE(tape.Read(value));
    EXPECT_EQ(value, 20);
}

TEST_F(TapeTest, MoveBackwardOutOfBounds) {
    Tape tape(test_file_, delays_);

    EXPECT_THROW({ tape.Move(MoveDirection::kBackward); }, std::out_of_range);
}

TEST_F(TapeTest, MoveForwardBeyondEOF) {
    Tape tape(test_file_, delays_);

    EXPECT_NO_THROW({ tape.Move(MoveDirection::kForward); });

    int32_t value;
    EXPECT_FALSE(tape.Read(value));
}

TEST_F(TapeTest, DelaysAreApplied) {
    delays_.read_delay_ms_ = std::chrono::milliseconds(10);
    delays_.write_delay_ms_ = std::chrono::milliseconds(10);
    delays_.move_delay_ms_ = std::chrono::milliseconds(10);
    delays_.rewind_delay_ms_ = std::chrono::milliseconds(10);

    auto start = std::chrono::high_resolution_clock::now();

    Tape tape(test_file_, delays_);
    int32_t value = 42;
    tape.Write(value);
    tape.Rewind();
    tape.Read(value);
    tape.Move(MoveDirection::kForward);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_GE(duration.count(), 40);
}
