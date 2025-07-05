add_library(VORTEK_PHYSICS
    "include/Physics/Box2DWrappers.h"
    "src/Box2DWrappers.cpp"
    "include/Physics/ContactListener.h"
    "src/ContactListener.cpp"
    "include/Physics/UserData.h"
    "src/UserData.cpp"
	"include/Physics/PhysicsUtilities.h"
	"src/PhysicsUtilities.cpp"
	"include/Physics/BoxTraceCallback.h"
 "src/BoxTraceCallback.cpp" "include/Physics/RayCastCallback.h" "src/RayCastCallback.cpp")

target_include_directories(
    VORTEK_PHYSICS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(VORTEK_PHYSICS
    PRIVATE VORTEK_LOGGER
	PRIVATE VORTEK_UTILITIES
    PUBLIC box2d::box2d)

target_compile_options(
    VORTEK_PHYSICS PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
