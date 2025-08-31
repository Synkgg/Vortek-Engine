add_executable(
	VORTEK_ENGINE

	"src/main.cpp"
	"src/Runtime.h"
	"src/Runtime.cpp"
)


target_link_libraries(VORTEK_ENGINE
	PRIVATE VORTEK_CORE
	libzippp::libzippp
)

target_compile_options(
    VORTEK_ENGINE
	PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)

target_precompile_headers(VORTEK_ENGINE REUSE_FROM PCH)
