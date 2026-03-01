#pragma once
#include "Rendering/Buffers/Framebuffer.h"

namespace Vortek::Editor
{
enum class FramebufferType
{
	TILEMAP,
	SCENE,
	NO_TYPE
};

struct EditorFramebuffers
{
	std::map<FramebufferType, std::shared_ptr<Vortek::Rendering::Framebuffer>> mapFramebuffers;
};
} // namespace Vortek::Editor
