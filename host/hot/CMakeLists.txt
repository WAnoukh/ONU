include_directories(${INLUDES_PATH})

set(HOT_SOURCES 
    ./src/module.c
    ./src/module.h
)

if(ENABLE_HOT_RELOAD)
    add_library(hot SHARED ${HOT_SOURCES})
else()
    add_library(hot STATIC ${HOT_SOURCES})
endif()

target_link_libraries(hot
    core
    game
)

target_include_directories(hot
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../shared/
)

set_target_properties(hot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
