add_executable(VORTEK_CRASH_REPORTER "src/main.cpp")

target_include_directories(
    VORTEK_CRASH_REPORTER PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	VORTEK_CRASH_REPORTER
	PRIVATE  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	glad::glad
	soil2
	imgui
)
