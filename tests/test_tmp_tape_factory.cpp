#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "tmp_tape_factory.h"
#include "tape.h"

class TmpTapeFactoryTest : public testing::Test {
protected:
    void SetUp() override {
        test_dir_ = std::filesystem::temp_directory_path() / "tape_test_dir";
        std::filesystem::create_directories(test_dir_);
    }

    void TearDown() override {
        std::filesystem::remove_all(test_dir_);
    }

    std::filesystem::path test_dir_;
    TapeDelays test_delays_{std::chrono::milliseconds(1), std::chrono::milliseconds(2), std::chrono::milliseconds(3)};
};

bool FileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

TEST_F(TmpTapeFactoryTest, ConstructorCreatesDirectory) {
    const std::string new_dir = test_dir_.string() + "/new_subdir";
    std::filesystem::remove_all(new_dir);
    TmpTapeFactory factory(new_dir, test_delays_);

    EXPECT_TRUE(std::filesystem::exists(new_dir));
}

TEST_F(TmpTapeFactoryTest, CreateReturnsTapeAndTracksFile) {
    TmpTapeFactory factory(test_dir_.string(), test_delays_);
    const auto tape = factory.Create();

    ASSERT_NE(tape, nullptr);

    const bool has_files = !std::filesystem::is_empty(test_dir_);
    EXPECT_TRUE(has_files);
}

TEST_F(TmpTapeFactoryTest, GeneratesTapeNamesWithIncreasingCounter) {
    TmpTapeFactory factory(test_dir_.string(), test_delays_);

    auto tape1 = factory.Create();
    auto tape2 = factory.Create();
    auto tape3 = factory.Create();

    const size_t file_count = std::distance(
        std::filesystem::directory_iterator(test_dir_),
        std::filesystem::directory_iterator{}
    );
    EXPECT_EQ(file_count, 3);
}

TEST_F(TmpTapeFactoryTest, DestructorRemovesFiles) {
    {
        TmpTapeFactory factory(test_dir_.string(), test_delays_);
        factory.Create();
        factory.Create();

        const size_t file_count = std::distance(
        std::filesystem::directory_iterator(test_dir_),
        std::filesystem::directory_iterator{}
        );
        EXPECT_EQ(file_count, 2);
    }
    const size_t file_count = std::distance(
        std::filesystem::directory_iterator(test_dir_),
        std::filesystem::directory_iterator{}
    );
    EXPECT_EQ(file_count, 0);
}

TEST_F(TmpTapeFactoryTest, CleanupTempFilesRemovesFiles) {
    class TestableFactory : public TmpTapeFactory {
    public:
        using TmpTapeFactory::TmpTapeFactory;
        using TmpTapeFactory::CleanupTempFiles;
    };
    TestableFactory factory(test_dir_.string(), test_delays_);
    factory.Create();
    factory.Create();
    size_t file_count = std::distance(
        std::filesystem::directory_iterator(test_dir_),
        std::filesystem::directory_iterator{}
    );
    EXPECT_EQ(file_count, 2);

    factory.CleanupTempFiles();
    file_count = std::distance(
        std::filesystem::directory_iterator(test_dir_),
        std::filesystem::directory_iterator{}
    );
    EXPECT_EQ(file_count, 0);
}
