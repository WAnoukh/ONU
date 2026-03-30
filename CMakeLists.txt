cmake_minimum_required(VERSION 3.31)
project(Project C CXX)

option(ENABLE_HOT_RELOAD "Enable hot reload support" ON)

if(ENABLE_HOT_RELOAD)
    add_compile_definitions(ENABLE_HOT_RELOAD=1)
else()
    add_compile_definitions(ENABLE_HOT_RELOAD=0)
endif()

set(CMAKE_USE_RELATIVE_PATHS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

if(EXISTS "${CMAKE_SOURCE_DIR}/CMakeLocal.cmake")
    include("${CMAKE_SOURCE_DIR}/CMakeLocal.cmake")
endif()

add_subdirectory(core/)
add_subdirectory(host/exe/)
add_subdirectory(host/hot/)

add_subdirectory(engine/)
add_subdirectory(game/)

############
### Glad ###
############

find_package(OpenGL REQUIRED)

add_library(glad SHARED)

target_sources(glad
    PUBLIC
    ${INCLUDES_PATH}/glad/glad.h
    PRIVATE
    ${INCLUDES_PATH}/glad/glad.c
)

target_compile_definitions(glad PRIVATE 
    GLAD_GLAPI_EXPORT=1 
    GLAD_GLAPI_EXPORT_BUILD=1
)

target_include_directories(glad PUBLIC 
    ${INCLUDES_PATH}
)

target_link_libraries(glad PRIVATE 
    opengl32
)

set_target_properties(glad 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

#############
### ImGui ###
#############
include_directories(${INCLUDES_PATH})

add_library(cimgui STATIC
    ${CIMGUI_PATH}/cimgui.cpp
    ${CIMGUI_PATH}/cimgui_impl.cpp
    ${CIMGUI_PATH}/imgui/imgui.cpp
    ${CIMGUI_PATH}/imgui/imgui_draw.cpp
    ${CIMGUI_PATH}/imgui/imgui_tables.cpp
    ${CIMGUI_PATH}/imgui/imgui_widgets.cpp
    # optional:
    ${CIMGUI_PATH}/imgui/imgui_demo.cpp
)

target_compile_definitions(glad PUBLIC 
    CIMGUI_DEFINE_ENUMS_AND_STRUCTS=1 
    CIMGUI_USE_OPENGL3=1
    CIMGUI_USE_GLFW=1
)

set_target_properties(cimgui PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(cimgui 
    PUBLIC
    ${CIMGUI_PATH}
    ${CIMGUI_PATH}/imgui
)

target_compile_definitions(cimgui PUBLIC 
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_DLL
    "-DIMGUI_IMPL_API=extern \"C\" __declspec(dllexport)"
)

target_link_libraries(cimgui PRIVATE
    ${LIBS_PATH}/MinGW/libglfw3dll.a
    opengl32
    gdi32
    user32
    kernel32
)

add_custom_command(TARGET cimgui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/externals/glfw3.dll"
    "${CMAKE_BINARY_DIR}/bin/glfw3.dll"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET cimgui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "${CMAKE_BINARY_DIR}/bin/resources"
        COMMENT "Copying resources next to exe"
    )
endif()
