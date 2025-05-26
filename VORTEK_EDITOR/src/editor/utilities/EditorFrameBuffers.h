#pragma once 
#include "Rendering/Buffers/Framebuffer.h"
#include <map>

namespace VORTEK_EDITOR
{
	enum class FramebufferType
	{
		TILEMAP, SCENE, NO_TYPE
	};

	struct EditorFramebuffers
	{
		std::map<FramebufferType, std::shared_ptr<VORTEK_RENDERING::Framebuffer>> mapFramebuffers;
	};
}