#include "SoundBindings.h"
#include "../ECS/Registry.h"
#include "../ECS/MainRegistry.h"
#include "../Resources/AssetManager.h"
#include <Sounds/MusicPlayer/MusicPlayer.h>
#include <Sounds/SoundPlayer/SoundFxPlayer.h>
#include <Logger/Logger.h>

using namespace VORTEK_SOUNDS;
using namespace VORTEK_RESOURCES;

void VORTEK_CORE::Scripting::SoundBinder::CreateSoundBind(sol::state& lua)
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& musicPlayer = mainRegistry.GetMusicPlayer();
	auto& assetManager = mainRegistry.GetAssetManager();

	lua.new_usertype<MusicPlayer>(
		"Music",
		sol::no_constructor,
		"play",
		sol::overload(
			[&](const std::string& musicName, int loops) {
				auto pMusic = assetManager.GetMusic(musicName);
				if (!pMusic)
				{
					VORTEK_ERROR("Failed to get music [{}] - From the asset maanger!", musicName);
					return;
				}
				musicPlayer.Play(*pMusic, loops);
			},
			[&](const std::string& musicName) {
				auto pMusic = assetManager.GetMusic(musicName);
				if (!pMusic)
				{
					VORTEK_ERROR("Failed to get music [{}] - From the asset maanger!", musicName);
					return;
				}
				musicPlayer.Play(*pMusic, -1);
			}),
		"stop",
		[&]() { musicPlayer.Stop(); },
		"pause",
		[&]() { musicPlayer.Pause(); },
		"resume",
		[&]() { musicPlayer.Resume(); },
		"set_volume",
		[&](int volume) { musicPlayer.SetVolume(volume); },
		"is_playing",
		[&]() { return musicPlayer.IsPlaying(); });

	// Create the SoundFxPlayer Bindings
	auto& soundFxPlayer = mainRegistry.GetSoundPlayer();

	lua.new_usertype<SoundFxPlayer>(
		"Sound",
		sol::no_constructor,
		"play",
		sol::overload(
			[&](const std::string& soundName) {
				auto pSoundFx = assetManager.GetSoundFx(soundName);
				if (!pSoundFx)
				{
					VORTEK_ERROR("Failed to get [{}] from the Asset Manager", soundName);
					return;
				}

				soundFxPlayer.Play(*pSoundFx);
			},
			[&](const std::string& soundName, int loops, int channel) {
				auto pSoundFx = assetManager.GetSoundFx(soundName);
				if (!pSoundFx)
				{
					VORTEK_ERROR("Failed to get [{}] from the Asset Manager", soundName);
					return;
				}

				soundFxPlayer.Play(*pSoundFx, loops, channel);
			}),
		"stop",
		[&](int channel) { soundFxPlayer.Stop(channel); },
		"set_volume",
		[&](int channel, int volume) { soundFxPlayer.SetVolume(channel, volume); },
		"is_playing",
		[&](int channel) { return soundFxPlayer.IsPlaying(channel); });
}