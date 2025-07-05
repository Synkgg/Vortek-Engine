#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE::Scripting
{
struct GLMBindings
{
	static void CreateGLMBindings( sol::state& lua );
};
} // namespace VORTEK_CORE::Scripting
