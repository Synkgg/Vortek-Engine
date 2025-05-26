add_library(VORTEK_WINDOW
	"src/Windowing/Inputs/Button.h"
	"src/Windowing/Inputs/Button.cpp"
	"src/Windowing/Inputs/Gamepad.h"
	"src/Windowing/Inputs/Gamepad.cpp"
	"src/Windowing/Inputs/GPButtons.h"
	"src/Windowing/Inputs/Keyboard.h"
	"src/Windowing/Inputs/Keyboard.cpp"
	"src/Windowing/Inputs/Keys.h"
	"src/Windowing/Inputs/Mouse.h"
	"src/Windowing/Inputs/Mouse.cpp"
	"src/Windowing/Inputs/MouseButtons.h"
	
	"src/Windowing/Window/Window.h"
	"src/Windowing/Window/Window.cpp"
)

target_include_directories(
    VORTEK_WINDOW
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	VORTEK_WINDOW
	PUBLIC VORTEK_UTILITIES
)

target_compile_options(
    VORTEK_WINDOW PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
