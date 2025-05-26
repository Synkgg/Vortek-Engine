add_library(VORTEK_PHYSICS
    "src/Physics/Box2DWrappers.h"
    "src/Physics/Box2DWrappers.cpp"
    "src/Physics/ContactListener.h"
    "src/Physics/ContactListener.cpp"
    "src/Physics/UserData.h"
    "src/Physics/UserData.cpp"
	"src/Physics/PhysicsUtilities.h"
	"src/Physics/PhysicsUtilities.cpp"
)

target_include_directories(
    VORTEK_PHYSICS
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(VORTEK_PHYSICS
    PRIVATE VORTEK_LOGGER
	PRIVATE VORTEK_UTILITIES
    PUBLIC box2d::box2d
)

target_compile_options(
    VORTEK_PHYSICS PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
