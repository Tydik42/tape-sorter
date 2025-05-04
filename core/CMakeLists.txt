set(HEADERS
        tape_config.h
        i_tape.h
        tape.h
)

set(SOURCES
        tape_config.cpp
        tape.cpp
)

add_library(${PROJECT_NAME}-core ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME}-core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
