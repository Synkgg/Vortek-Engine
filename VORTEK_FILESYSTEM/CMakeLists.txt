add_library(
	VORTEK_FILESYSTEM
	# Dialogs
	"src/Dialogs/FileDialog.h"
	"src/Dialogs/FileDialog.cpp"
	
	# Serializers
	"src/Serializers/JSONSerializer.h"
	"src/Serializers/JSONSerializer.inl"
	"src/Serializers/JSONSerializer.cpp"
)

target_include_directories(
    VORTEK_FILESYSTEM
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	VORTEK_FILESYSTEM
	PRIVATE
	VORTEK_LOGGER
	tinyfiledialogs::tinyfiledialogs
	rapidjson
	VORTEK_UTILITIES
)

target_compile_options(
	VORTEK_FILESYSTEM
	PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
