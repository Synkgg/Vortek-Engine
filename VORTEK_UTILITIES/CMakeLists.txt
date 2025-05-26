add_library(VORTEK_UTILITIES
	"src/VortekUtilities/HelperUtilities.h"
	"src/VortekUtilities/RandomGenerator.h"
	"src/VortekUtilities/RandomGenerator.cpp"
	"src/VortekUtilities/SDL_Wrappers.h"
	"src/VortekUtilities/SDL_Wrappers.cpp"
	"src/VortekUtilities/Timer.h"
	"src/VortekUtilities/Timer.cpp"
	"src/VortekUtilities/VortekUtilities.h"
)

target_include_directories(
    VORTEK_UTILITIES
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	VORTEK_UTILITIES
	PUBLIC VORTEK_LOGGER SDL2::SDL2
)

target_compile_options(
    VORTEK_UTILITIES PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
