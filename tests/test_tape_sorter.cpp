#include <algorithm>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "i_tape.h"
#include "tape_sorter.h"

class MemoryTape : public ITape {
public:
    explicit MemoryTape(std::vector<int32_t> const& initial_data = {})
        : data_(initial_data), position_(0) {}

    bool Read(int32_t& value) override {
        if (position_ >= data_.size()) {
            return false;
        }
        value = data_[position_];
        return true;
    }

    void Write(int32_t value) override {
        if (position_ >= data_.size()) {
            data_.push_back(value);
        } else {
            data_[position_] = value;
        }
    }

    void Move(MoveDirection direction) override {
        if (direction == MoveDirection::kForward) {
            position_++;
        } else if (position_ > 0) {
            position_--;
        } else {
            throw std::out_of_range("Cannot move backward at position 0");
        }
    }

    void Rewind() override {
        position_ = 0;
    }

    [[nodiscard]] std::vector<int32_t> const& GetData() const {
        return data_;
    }

private:
    std::vector<int32_t> data_;
    size_t position_;
};

class MemoryTapeFactory : public ITapeFactory {
public:
    std::unique_ptr<ITape> Create() override {
        return std::make_unique<MemoryTape>();
    }
};

class TapeSorterTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory_ = std::make_unique<MemoryTapeFactory>();
    }

    std::unique_ptr<MemoryTapeFactory> factory_;
};

TEST_F(TapeSorterTest, SortHandlesEmptyInput) {
    auto input_tape = std::make_unique<MemoryTape>();
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::move(factory_));
    sorter.Sort(*input_tape, *output_tape);

    EXPECT_TRUE(output_tape->GetData().empty());
}

TEST_F(TapeSorterTest, SortWithSingleValue) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{42});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    ASSERT_EQ(output_tape->GetData().size(), 1);
    EXPECT_EQ(output_tape->GetData()[0], 42);
}

TEST_F(TapeSorterTest, SortCorrectlySortsSmallInput) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{5, 3, 8, 1, 6});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {1, 3, 5, 6, 8};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}

TEST_F(TapeSorterTest, SortMaintainsAlreadySortedInput) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{1, 2, 3, 4, 5});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {1, 2, 3, 4, 5};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}

TEST_F(TapeSorterTest, SortHandlesReverseSortedInput) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{5, 4, 3, 2, 1});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {1, 2, 3, 4, 5};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}

TEST_F(TapeSorterTest, SortHandlesDuplicateValues) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{3, 1, 3, 2, 1});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {1, 1, 2, 3, 3};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}

TEST_F(TapeSorterTest, SortWithMultipleBlocks) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{9, 7, 5, 3, 1, 8, 6, 4, 2});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(5, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}

TEST_F(TapeSorterTest, SortWithSingleValueBlockSize) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{3, 1, 4, 2});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(1, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {1, 2, 3, 4};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}

TEST_F(TapeSorterTest, SortHandlesNegativeValues) {
    auto input_tape = std::make_unique<MemoryTape>(std::vector<int32_t>{3, -5, 0, -2, 1});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {-5, -2, 0, 1, 3};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}

TEST_F(TapeSorterTest, SortHandlesIntMinMaxValues) {
    auto input_tape =
            std::make_unique<MemoryTape>(std::vector<int32_t>{INT32_MAX, 0, INT32_MIN, 42});
    auto output_tape = std::make_unique<MemoryTape>();

    TapeSorter sorter(10, std::make_unique<MemoryTapeFactory>());
    sorter.Sort(*input_tape, *output_tape);

    std::vector<int32_t> expected = {INT32_MIN, 0, 42, INT32_MAX};
    ASSERT_EQ(output_tape->GetData().size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(output_tape->GetData()[i], expected[i]);
    }
}
