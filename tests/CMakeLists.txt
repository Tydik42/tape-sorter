include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

set(TEST_TARGET_NAME ${PROJECT_NAME}-test)

set(TEST_SOURCES
        test_config_parser.cpp
        test_tape.cpp
        test_tmp_tape_factory.cpp
        test_tape_sorter.cpp
)

add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES})

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
        ${PROJECT_NAME}-core
        gtest
        gtest_main
)

gtest_discover_tests(${TEST_TARGET_NAME}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

set_target_properties(${TEST_TARGET_NAME} PROPERTIES FOLDER tests)
