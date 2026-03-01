#pragma once
#include <sol/sol.hpp>

namespace Vortek::Core::Scripting
{
struct GLMBindings
{
	static void CreateGLMBindings( sol::state& lua );
};
} // namespace Vortek::Core::Scripting
