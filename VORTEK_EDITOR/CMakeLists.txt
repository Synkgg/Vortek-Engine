add_executable(
	VORTEK_EDITOR
	# Editor - Commands
	"src/editor/commands/cmds/CreateTileToolCmds.h"
	"src/editor/commands/cmds/CreateTileToolCmds.cpp"
	"src/editor/commands/cmds/RectToolCmds.h"
	"src/editor/commands/cmds/RectToolCmds.cpp"
	"src/editor/commands/AbstractCommandManager.h"
	"src/editor/commands/AbstractCommandManager.inl"
	"src/editor/commands/CommandManager.h"
	
	# Editor - Displays
	"src/editor/displays/AssetDisplay.h"
	"src/editor/displays/AssetDisplay.cpp"
	"src/editor/displays/EditorStyleToolDisplay.h"
	"src/editor/displays/EditorStyleToolDisplay.cpp"
	"src/editor/displays/IDisplay.h"
	"src/editor/displays/LogDisplay.h"
	"src/editor/displays/LogDisplay.cpp"
	"src/editor/displays/MenuDisplay.h"
	"src/editor/displays/MenuDisplay.cpp"
	"src/editor/displays/SceneDisplay.h"
	"src/editor/displays/SceneDisplay.cpp"
	"src/editor/displays/SceneHierarchyDisplay.h"
	"src/editor/displays/SceneHierarchyDisplay.cpp"
	"src/editor/displays/TileDetailsDisplay.h"
	"src/editor/displays/TileDetailsDisplay.cpp"
	"src/editor/displays/TilemapDisplay.h"
	"src/editor/displays/TilemapDisplay.cpp"
	"src/editor/displays/TilesetDisplay.h"
	"src/editor/displays/TilesetDisplay.cpp"
	
	# Scene 
	"src/editor/scene/SceneManager.h"
	"src/editor/scene/SceneManager.cpp"
	"src/editor/scene/SceneObject.h"
	"src/editor/scene/SceneObject.cpp"
	
	# Systems
	"src/editor/systems/GridSystem.h"
	"src/editor/systems/GridSystem.cpp"
	
	# Tools 
	"src/editor/tools/AbstractTool.h"
	"src/editor/tools/AbstractTool.cpp"
	"src/editor/tools/CreateTileTool.h"
	"src/editor/tools/CreateTileTool.cpp"
	"src/editor/tools/RectFillTool.h"
	"src/editor/tools/RectFillTool.cpp"
	"src/editor/tools/TileTool.h"
	"src/editor/tools/TileTool.cpp"
	"src/editor/tools/ToolAccessories.h"
	"src/editor/tools/ToolManager.h"
	"src/editor/tools/ToolManager.cpp"
	
	# Utilities
	"src/editor/utilities/fonts/IconsFontAwesome5.h"
	"src/editor/utilities/DrawComponentUtils.h"
	"src/editor/utilities/DrawComponentUtils.inl"
	"src/editor/utilities/DrawComponentUtils.cpp"
	"src/editor/utilities/editor_textures.h"
	"src/editor/utilities/EditorFrameBuffers.h"
	"src/editor/utilities/EditorUtilities.h"
	"src/editor/utilities/ImGuiUtils.h"
	"src/editor/utilities/ImGuiUtils.cpp"
	
	# Main Application
	"src/Application.h"
	"src/Application.cpp"
	"src/main.cpp"
)

target_sources(
	VORTEK_EDITOR PRIVATE
	../thirdparty/imgui_backends/imgui_impl_sdl2.cpp
)

target_include_directories(
    VORTEK_EDITOR PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE ../thirdparty/imgui_backends
)

target_link_libraries(VORTEK_EDITOR PRIVATE VORTEK_CORE imgui::imgui)

add_resources(TARGET Assets IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets OUT_DIR assets)
add_dependencies(VORTEK_EDITOR Copy-Assets)

set_target_properties(VORTEK_EDITOR PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
)

target_compile_options(
    VORTEK_EDITOR PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
