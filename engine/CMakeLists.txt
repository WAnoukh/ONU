include_directories(${INLUDES_PATH})

add_library(engine STATIC)

target_sources(engine 
    PRIVATE 
    ./src/engine.c
    ./src/engine_commands.c

    ./src/console/log.c
    ./src/console/console.c
    ./src/console/imgui_console.c
    ./src/console/command/commands.c
    ./src/console/command/command_registry.c
    ./src/console/command/console_input_parser.c

    ./src/rendering/camera.c
    ./src/rendering/td_camera.c
    ./src/rendering/shader.c
    ./src/rendering/texture.c
    ./src/rendering/rendering.c
    ./src/rendering/material.c

    ./src/transform/transform.c

    ${CIMGUI_PATH}/imgui/backends/imgui_impl_glfw.cpp
    ${CIMGUI_PATH}/imgui/backends/imgui_impl_opengl3.cpp
)

find_package(OpenGL REQUIRED)

target_compile_definitions(engine PRIVATE
    GLFW_DLL
)

target_compile_definitions(engine PUBLIC 
    CGLM_FORCE_LEFT_HANDED
)

target_link_libraries(engine
    ${LIBS_PATH}/MinGW/libglfw3dll.a
    glad
    opengl32
    gdi32
    user32
    kernel32

    core
    cimgui
    game
)

target_include_directories(engine
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${INLUDES_PATH}/
)

set_target_properties(engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
