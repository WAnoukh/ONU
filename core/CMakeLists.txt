include_directories(${INLUDES_PATH})

add_library(core SHARED)

target_sources(core 
    PRIVATE 
    ./src/memory/arena.c
    ./src/memory/arena.h

    ./src/string/BString.c
    ./src/string/BString.h
    ./src/string/StringView.c
    ./src/string/StringView.h

    ./src/inputs.h
)

target_include_directories(core
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

set_target_properties(core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
