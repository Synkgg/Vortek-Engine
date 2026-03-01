#include "Sounds/Essentials/Music.h"

Vortek::Sounds::Music::Music( const SoundParams& params, MusicPtr pMusic )
	: m_Params{ params }
	, m_pMusic{ std::move( pMusic ) }
{
}
