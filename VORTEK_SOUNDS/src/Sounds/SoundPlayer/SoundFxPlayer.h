#pragma once

namespace VORTEK_SOUNDS {
	class SoundFxPlayer
	{
	public:
		SoundFxPlayer() = default;
		~SoundFxPlayer() = default;

		void Play(class SoundFX& soundFx);
		void Play(class SoundFX& soundFx, int loops, int channel = -1);
		void SetVolume(int volume, int channel = -1);
		void Stop(int channel);
		bool IsPlaying(int channel);
	};
}