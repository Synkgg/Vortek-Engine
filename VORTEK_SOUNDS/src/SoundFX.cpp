#include "Sounds/Essentials/SoundFX.h"

VORTEK_SOUNDS::SoundFX::SoundFX( const SoundParams& params, SoundFxPtr pSoundFx )
	: m_Params{ params }
	, m_pSoundFx{ std::move( pSoundFx ) }
{
}
