include_directories(${INLUDES_PATH})

add_library(game STATIC)

target_sources(game 
    PRIVATE 
    ./src/game.c
    ./src/game_commands.c
    ./src/level.c
    ./src/history.c
    ./src/tilemap.c
    ./src/gamestate.c
    ./src/transform.h
    ./src/game_runtime.c
    ./src/level_sequence.c
    ./src/serialization.c

    ./src/editor/editor.c
    ./src/editor/editor_context.c
    ./src/editor/editor_interface.c

)

target_link_libraries(game
    core
    engine
)

target_include_directories(game
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${INLUDES_PATH}/
)

set_target_properties(game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
