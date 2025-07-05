if(WIN32)
	set(FILE_PROCESSOR_PATH "src/FileProcessor_Win.cpp")
else()
	set(FILE_PROCESSOR_PATH "src/FileProcessor_Unix.cpp")
endif()

add_library(VORTEK_FILESYSTEM
	# Dialogs
	"include/VortekFilesystem/Dialogs/FileDialog.h"
	"src/FileDialog.cpp"

	# Serializers
	"include/VortekFilesystem/Serializers/JSONSerializer.h"
	"include/VortekFilesystem/Serializers/JSONSerializer.inl"
	"src/JSONSerializer.cpp"
	"include/VortekFilesystem/Serializers/LuaSerializer.h"
	"include/VortekFilesystem/Serializers/LuaSerializer.inl"
	"src/LuaSerializer.cpp"

	# Process
	"include/VortekFilesystem/Process/FileProcessor.h"
	${FILE_PROCESSOR_PATH}

	# Utilities
	"include/VortekFilesystem/Utilities/DirectoryWatcher.h"
	"src/DirectoryWatcher.cpp"
)

target_include_directories(
	VORTEK_FILESYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
	VORTEK_FILESYSTEM PRIVATE VORTEK_LOGGER tinyfiledialogs::tinyfiledialogs rapidjson VORTEK_UTILITIES
)

target_compile_options(
	VORTEK_FILESYSTEM PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
