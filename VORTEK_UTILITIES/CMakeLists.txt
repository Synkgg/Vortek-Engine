add_library(VORTEK_UTILITIES
    "include/VortekUtilities/RandomGenerator.h"
    "src/RandomGenerator.cpp"
    "include/VortekUtilities/SDL_Wrappers.h"
    "src/SDL_Wrappers.cpp"
    "include/VortekUtilities/Timer.h"
    "src/Timer.cpp" 
    "include/VortekUtilities/VortekUtilities.h"
	"src/VortekUtilities.cpp"
	"include/VortekUtilities/HelperUtilities.h"
	"include/VortekUtilities/MathUtilities.h"
	"include/VortekUtilities/Tween.h"
	"src/Tween.cpp"
)

target_include_directories(
    VORTEK_UTILITIES PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(VORTEK_UTILITIES PUBLIC VORTEK_LOGGER SDL2::SDL2)

target_compile_options(
    VORTEK_UTILITIES PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
