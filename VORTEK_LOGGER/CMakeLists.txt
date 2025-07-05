add_library(VORTEK_LOGGER
    "include/Logger/Logger.h"
    "include/Logger/Logger.inl"
    "src/Logger.cpp"
	"include/Logger/CrashLogger.h"
	"src/CrashLogger.cpp"
)

target_include_directories(
    VORTEK_LOGGER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(VORTEK_LOGGER PUBLIC fmt::fmt)

target_compile_options( 
    VORTEK_LOGGER PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>)
