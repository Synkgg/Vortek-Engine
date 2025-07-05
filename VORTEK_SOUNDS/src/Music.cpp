#include "Sounds/Essentials/Music.h"
#include <Logger/Logger.h>

VORTEK_SOUNDS::Music::Music( const SoundParams& params, MusicPtr pMusic )
	: m_Params{ params }
	, m_pMusic{ std::move( pMusic ) }
{
}
