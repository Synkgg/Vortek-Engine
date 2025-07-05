#pragma once
#include "SoundParams.h"
#include <VORTEKUtilities/SDL_Wrappers.h>

namespace VORTEK_SOUNDS
{
class SoundFX
{
  public:
	SoundFX( const SoundParams& params, SoundFxPtr pSoundFx );
	~SoundFX() = default;

	inline const std::string& GetName() const { return m_Params.name; }
	inline const std::string& GetDescription() const { return m_Params.description; }
	inline const std::string& GetFilename() const { return m_Params.filename; }
	inline const double GetDuration() const { return m_Params.duration; }
	inline Mix_Chunk* GetSoundFxPtr() const
	{
		if ( !m_pSoundFx )
			return nullptr;
		return m_pSoundFx.get();
	}

  private:
	SoundParams m_Params;
	SoundFxPtr m_pSoundFx;
};
} // namespace VORTEK_SOUNDS
