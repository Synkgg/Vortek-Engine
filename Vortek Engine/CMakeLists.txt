cmake_minimum_required(VERSION 3.26)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

project(Vortek_Engine)

if(PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(
        FATAL_ERROR
        "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# OpenGL realtime debugger settings
set( VORTEK_OPENGL_DEBUG_CALLBACK					ON	)
set( VORTEK_OPENGL_DEBUG_FORWARD_COMPATIBILITY		ON	)

# Force discrete GPU on laptops fitted with Optimus / dual GPUs technology
set( VORTEK_OPENGL_FORCE_DISCRETE_GPU	OFF	)

include(cmake/CompilerSettings.cmake)
include(cmake/Options.cmake)

set(BUILD_SHARED_LIBS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # .lib, .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # .dll, .so
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") # .exe

if(WIN32)
    add_definitions(-DNOMINMAX)
	set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources/vortek.rc")
endif()

add_compile_definitions(IN_VORTEK_EDITOR)

add_compile_definitions(SOL_ALL_SAFETIES_ON=1)
add_compile_definitions(SOL_EXCEPTIONS_ALWAYS_UNSAFE=1)

find_package(glad CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
# Removed ImGui from Configs for now.
# Current VCPKG docking version is not up to date with
# Needed SDL2 changes for multi-viewports.
# Added entirely in the thirdparty/imgui folder.
# find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

add_subdirectory(VORTEK_LOGGER)
add_subdirectory(VORTEK_UTILITIES)
add_subdirectory(VORTEK_PHYSICS)
add_subdirectory(VORTEK_RENDERING)
add_subdirectory(VORTEK_FILESYSTEM)
add_subdirectory(VORTEK_SOUNDS)
add_subdirectory(VORTEK_WINDOW)
add_subdirectory(VORTEK_CORE)
set_target_properties(
    VORTEK_LOGGER
	VORTEK_UTILITIES
	VORTEK_PHYSICS
	VORTEK_RENDERING
	VORTEK_FILESYSTEM
	VORTEK_SOUNDS
	VORTEK_WINDOW
	VORTEK_CORE
    PROPERTIES FOLDER "Engine")

include(cmake/AddResources.cmake)
add_subdirectory(VORTEK_EDITOR)
