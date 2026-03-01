include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# -------------------------
# Dear ImGui
# -------------------------

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG docking
    GIT_SHALLOW 1
)

FetchContent_MakeAvailable(imgui)

file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/misc/cpp
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    glad::glad
)

# -------------------------
# ImGui Notify (header-only)
# -------------------------

set(IMGUI_NOTIFY_DIR "${CMAKE_SOURCE_DIR}/thirdparty/imgui-notify")

add_library(imgui_notify INTERFACE)
target_include_directories(imgui_notify INTERFACE
    ${IMGUI_NOTIFY_DIR}
    ${imgui_SOURCE_DIR}   # Needed so it finds imgui.h
)

# -------------------------
# ImGui Text Editor
# -------------------------

set(IMGUI_TEXT_EDITOR_DIR "${CMAKE_SOURCE_DIR}/thirdparty/TextEditor")

add_library(imgui_text_editor STATIC
    ${IMGUI_TEXT_EDITOR_DIR}/TextEditor.cpp
)

target_include_directories(imgui_text_editor PUBLIC
    ${IMGUI_TEXT_EDITOR_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imgui_SOURCE_DIR}/misc/cpp
)

target_link_libraries(imgui_text_editor PUBLIC imgui)
