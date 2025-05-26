add_library(
	VORTEK_LOGGER
	"src/Logger/Logger.h"
	"src/Logger/Logger.inl"
	"src/Logger/Logger.cpp"
)

target_include_directories(
    VORTEK_LOGGER
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	VORTEK_LOGGER
	PUBLIC fmt::fmt
)

target_compile_options( 
    VORTEK_LOGGER PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
