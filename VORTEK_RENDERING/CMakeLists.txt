add_library(VORTEK_RENDERING
	# Buffers
	"src/Rendering/Buffers/FrameBuffer.h"
	"src/Rendering/Buffers/FrameBuffer.cpp"
	
	# Core
	"src/Rendering/Core/Batcher.h"
	"src/Rendering/Core/BatchRenderer.h"
	"src/Rendering/Core/BatchRenderer.cpp"
	"src/Rendering/Core/Camera2D.h"
	"src/Rendering/Core/Camera2D.cpp"
	"src/Rendering/Core/CircleBatchRenderer.h"
	"src/Rendering/Core/CircleBatchRenderer.cpp"
	"src/Rendering/Core/LineBatchRenderer.h"
	"src/Rendering/Core/LineBatchRenderer.cpp"
	"src/Rendering/Core/RectBatchRenderer.h"
	"src/Rendering/Core/RectBatchRenderer.cpp"
	"src/Rendering/Core/Renderer.h"
	"src/Rendering/Core/Renderer.cpp"
	"src/Rendering/Core/TextBatchRenderer.h"
	"src/Rendering/Core/TextBatchRenderer.cpp"
	
	# Essentials
	"src/Rendering/Essentials/BatchTypes.h"
	"src/Rendering/Essentials/Font.h"
	"src/Rendering/Essentials/Font.cpp"
	"src/Rendering/Essentials/FontLoader.h"
	"src/Rendering/Essentials/FontLoader.cpp"
	"src/Rendering/Essentials/Primitives.h"
	"src/Rendering/Essentials/Shader.h"
	"src/Rendering/Essentials/Shader.cpp"
	"src/Rendering/Essentials/ShaderLoader.h"
	"src/Rendering/Essentials/ShaderLoader.cpp"
	"src/Rendering/Essentials/Texture.h"
	"src/Rendering/Essentials/Texture.cpp"
	"src/Rendering/Essentials/TextureLoader.h"
	"src/Rendering/Essentials/TextureLoader.cpp"
	"src/Rendering/Essentials/Vertex.h"
)

target_include_directories(
    VORTEK_RENDERING
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(VORTEK_RENDERING
    PRIVATE VORTEK_LOGGER
    PUBLIC glm::glm glad::glad soil2
)

target_compile_options(
    VORTEK_RENDERING PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
