set(HEADERS
        tape_config.h
        i_tape.h
        tape.h
        tmp_tape_factory.h
        tape_sorter.h
)

set(SOURCES
        tape_config.cpp
        tape.cpp
        tmp_tape_factory.cpp
        tape_sorter.cpp
)

add_library(${PROJECT_NAME}-core ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}-core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
