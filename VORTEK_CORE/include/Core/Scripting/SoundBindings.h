#pragma once
#include <sol/sol.hpp>
namespace Vortek::Core
{
namespace ECS
{
class Registry;
}
} // namespace Vortek::Core

namespace Vortek::Core::Scripting
{
/*
 * @brief Binds the MusicPlayer and the SoundPlayer to Lua using Sol2.
 */
struct SoundBinder
{
	static void CreateSoundBind( sol::state& lua );
};
} // namespace Vortek::Core::Scripting
