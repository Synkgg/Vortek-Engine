#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE
{
namespace ECS
{
class Registry;
}
} // namespace VORTEK_CORE

namespace VORTEK_CORE::Scripting
{
/*
 * @brief Binds the MusicPlayer and the SoundPlayer to Lua using Sol2.
 */
struct SoundBinder
{
	static void CreateSoundBind( sol::state& lua );
};
} // namespace VORTEK_CORE::Scripting
