#include "Core/Resources/AssetManager.h"
#include "Core/Resources/fonts/default_fonts.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/ECS/Registry.h"

#include <Rendering/Essentials/TextureLoader.h>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Rendering/Essentials/FontLoader.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Essentials/Font.h>
#include <Sounds/Essentials/Music.h>
#include <Sounds/Essentials/SoundFX.h>

#include <VortekUtilities/VortekUtilities.h>
#include <VortekUtilities/SDL_Wrappers.h>
#include <Logger/Logger.h>
#include <SDL_image.h>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

namespace VORTEK_RESOURCES
{
AssetManager::AssetManager( bool bEnableFilewatcher )
	: m_bFileWatcherRunning{ bEnableFilewatcher }
{
	if ( bEnableFilewatcher )
	{
		m_WatchThread = std::jthread( &AssetManager::FileWatcher, this );
	}

#ifdef IN_VORTEK_EDITOR
	IMG_Init( IMG_INIT_PNG );
	m_mapCursors.emplace( "default", MakeSharedFromSDLType<Cursor>( SDL_GetDefaultCursor() ) );
#endif
}

AssetManager::~AssetManager()
{
	m_bFileWatcherRunning = false;
	if ( m_WatchThread.joinable() )
	{
		m_WatchThread.join();
	}
}

bool AssetManager::CreateDefaultFonts()
{
	if ( !AddFontFromMemory( "OpenSans", CoreFonts::g_OpenSans ) )
	{
		VORTEK_ERROR( "Failed to create opensans font." );
		return false;
	}

	if ( !AddFontFromMemory( "SourceSans", CoreFonts::g_SourceSans ) )
	{
		VORTEK_ERROR( "Failed to create sourcesans font." );
		return false;
	}

	// TODO: Add more default fonts.

	return true;
}

bool AssetManager::AddTexture( const std::string& textureName, const std::string& texturePath, bool pixelArt,
							   bool bTileset )
{
	// Check to see if the texture already exists
	if ( m_mapTextures.find( textureName ) != m_mapTextures.end() )
	{
		VORTEK_ERROR( "Failed to add texture [{0}] -- Already exists!", textureName );
		return false;
	}

	auto pTexture = Vortek::Rendering::TextureLoader::Create( pixelArt ? Vortek::Rendering::Texture::TextureType::PIXEL
																	 : Vortek::Rendering::Texture::TextureType::BLENDED,
															texturePath,
															bTileset );

	if ( !pTexture )
	{
		VORTEK_ERROR( "Failed to load texture [{0}] at path [{1}]", textureName, texturePath );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapTextures.emplace( textureName, std::move( pTexture ) );

	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };

		fs::path path{ texturePath };
		auto lastWrite = fs::last_write_time( path );
		if ( Vortek::Utilities::CheckContainsValue( m_FilewatchParams,
											 [ & ]( const auto& params ) { return params.sFilepath == texturePath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = textureName,
															  .sFilepath = texturePath,
															  .lastWrite = lastWrite,
															  .eType = Vortek::Utilities::AssetType::TEXTURE } );
		}
	}

	return bSuccess;
}

bool AssetManager::AddTextureFromMemory( const std::string& textureName, const unsigned char* imageData, size_t length,
										 bool pixelArt, bool bTileset )
{
	// Check to see if the Texture already exist
	if ( m_mapTextures.contains( textureName ) )
	{
		VORTEK_ERROR( "AssetManager: Texture [{}] -- Already exists!", textureName );
		return false;
	}

	auto pTexture = Vortek::Rendering::TextureLoader::CreateFromMemory( imageData, length, !pixelArt, bTileset );

	// Load the texture
	if ( !pTexture )
	{
		VORTEK_ERROR( "Unable to load texture [{}] from memory!", textureName );
		return false;
	}

	// Insert the texture into the map
	auto [ itr, bSuccess ] = m_mapTextures.emplace( textureName, std::move( pTexture ) );

	return bSuccess;
}

std::shared_ptr<Vortek::Rendering::Texture> AssetManager::GetTexture( const std::string& textureName )
{
	auto texItr = m_mapTextures.find( textureName );
	if ( texItr == m_mapTextures.end() )
	{
		VORTEK_ERROR( "Failed to get texture [{0}] -- Does not exist!", textureName );
		return nullptr;
	}

	return texItr->second;
}

std::vector<std::string> AssetManager::GetTilesetNames() const
{
	return Vortek::Utilities::GetKeys( m_mapTextures, []( const auto& pair ) { return pair.second->IsTileset(); } );
}

bool AssetManager::AddFont( const std::string& fontName, const std::string& fontPath, float fontSize )
{
	if ( m_mapFonts.contains( fontName ) )
	{
		VORTEK_ERROR( "Failed to add font [{0}] -- Already Exists!", fontName );
		return false;
	}

	auto pFont = Vortek::Rendering::FontLoader::Create( fontPath, fontSize );

	if ( !pFont )
	{
		VORTEK_ERROR( "Failed to add font [{}] at path [{}] -- to the asset manager!", fontName, fontPath );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapFonts.emplace( fontName, std::move( pFont ) );

	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };

		fs::path path{ fontPath };
		auto lastWrite = fs::last_write_time( path );
		if ( Vortek::Utilities::CheckContainsValue( m_FilewatchParams,
											 [ & ]( const auto& params ) { return params.sFilepath == fontPath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = fontName,
															  .sFilepath = fontPath,
															  .lastWrite = lastWrite,
															  .eType = Vortek::Utilities::AssetType::FONT } );
		}
	}

	return bSuccess;
}

bool AssetManager::AddFontFromMemory( const std::string& fontName, unsigned char* fontData, float fontSize )
{

	if ( m_mapFonts.contains( fontName ) )
	{
		VORTEK_ERROR( "Failed to add font [{0}] -- Already Exists!", fontName );
		return false;
	}

	auto pFont = Vortek::Rendering::FontLoader::CreateFromMemory( fontData, fontSize );

	if ( !pFont )
	{
		VORTEK_ERROR( "Failed to add font [{0}] from memory -- to the asset manager!", fontName );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapFonts.emplace( fontName, std::move( pFont ) );

	return bSuccess;
}

std::shared_ptr<Vortek::Rendering::Font> AssetManager::GetFont( const std::string& fontName )
{
	auto fontItr = m_mapFonts.find( fontName );
	if ( fontItr == m_mapFonts.end() )
	{
		VORTEK_ERROR( "Failed to get font [{0}] -- Does not exist!", fontName );
		return nullptr;
	}

	return fontItr->second;
}

bool AssetManager::AddShader( const std::string& shaderName, const std::string& vertexPath,
							  const std::string& fragmentPath )
{
	// Check to see if the shader already exists
	if ( m_mapShader.contains( shaderName ) )
	{
		VORTEK_ERROR( "Failed to add shader [{0}] -- Already Exists!", shaderName );
		return false;
	}

	// Create and load the shader
	auto pShader = Vortek::Rendering::ShaderLoader::Create( vertexPath, fragmentPath );

	if ( !pShader )
	{
		VORTEK_ERROR( "Failed to load Shader [{0}] at vert path [{1}] and frag path [{2}]",
					 shaderName,
					 vertexPath,
					 fragmentPath );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapShader.emplace( shaderName, std::move( pShader ) );

	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };

		fs::path pathVert{ vertexPath };
		auto lastWriteVert = fs::last_write_time( pathVert );
		if ( Vortek::Utilities::CheckContainsValue( m_FilewatchParams,
											 [ & ]( const auto& params ) { return params.sFilepath == vertexPath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = shaderName + "_vert",
															  .sFilepath = vertexPath,
															  .lastWrite = lastWriteVert,
															  .eType = Vortek::Utilities::AssetType::SHADER } );
		}

		fs::path pathFrag{ fragmentPath };
		auto lastWriteFrag = fs::last_write_time( pathFrag );
		if ( Vortek::Utilities::CheckContainsValue(
				 m_FilewatchParams, [ & ]( const auto& params ) { return params.sFilepath == fragmentPath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = shaderName + "_frag",
															  .sFilepath = fragmentPath,
															  .lastWrite = lastWriteFrag,
															  .eType = Vortek::Utilities::AssetType::SHADER } );
		}
	}
	return bSuccess;
}

bool AssetManager::AddShaderFromMemory( const std::string& shaderName, const char* vertexShader,
										const char* fragShader )
{
	if ( m_mapShader.contains( shaderName ) )
	{
		VORTEK_ERROR( "Failed to add shader - [{0}] -- Already exists!", shaderName );
		return false;
	}

	auto pShader = Vortek::Rendering::ShaderLoader::CreateFromMemory( vertexShader, fragShader );
	auto [ itr, bSuccess ] = m_mapShader.insert( std::make_pair( shaderName, std::move( pShader ) ) );

	return bSuccess;
}

std::shared_ptr<Vortek::Rendering::Shader> AssetManager::GetShader( const std::string& shaderName )
{
	auto shaderItr = m_mapShader.find( shaderName );
	if ( shaderItr == m_mapShader.end() )
	{
		VORTEK_ERROR( "Failed to get shader [{0}] -- Does not exist!", shaderName );
		return nullptr;
	}

	return shaderItr->second;
}

bool AssetManager::AddMusic( const std::string& musicName, const std::string& filepath )
{
	if ( m_mapMusic.contains( musicName ) )
	{
		VORTEK_ERROR( "Failed to add music [{}] -- Already exists!", musicName );
		return false;
	}

	Mix_Music* pMusic = Mix_LoadMUS( filepath.c_str() );

	if ( !pMusic )
	{
		std::string error{ Mix_GetError() };
		VORTEK_ERROR( "Failed to load [{}] at path [{}] -- Mixer Error: {}", musicName, filepath, error );
		return false;
	}

	// Create the sound parameters
	Vortek::Sounds::SoundParams params{
		.name = musicName, .filename = filepath, .duration = Mix_MusicDuration( pMusic ) };

	// Create the music Pointer
	auto pMusicPtr = std::make_shared<Vortek::Sounds::Music>( params, MusicPtr{ pMusic } );
	if ( !pMusicPtr )
	{
		VORTEK_ERROR( "Failed to create the music ptr for [{}]", musicName );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapMusic.emplace( musicName, std::move( pMusicPtr ) );

	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };

		fs::path path{ filepath };
		auto lastWrite = fs::last_write_time( path );
		if ( Vortek::Utilities::CheckContainsValue( m_FilewatchParams,
											 [ & ]( const auto& params ) { return params.sFilepath == filepath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = musicName,
															  .sFilepath = filepath,
															  .lastWrite = lastWrite,
															  .eType = Vortek::Utilities::AssetType::MUSIC } );
		}
	}

	return bSuccess;
}

bool AssetManager::AddMusicFromMemory( const std::string& musicName, const unsigned char* musicData, size_t dataSize )
{
	if ( m_mapMusic.contains( musicName ) )
	{
		VORTEK_ERROR( "Failed to add music [{}] -- Already exists!", musicName );
		return false;
	}

	SDL_RWops* rw = SDL_RWFromMem( (void*)musicData, static_cast<int>( dataSize ) );
	Mix_MusicType type = DetectAudioFormat( musicData, dataSize );

	if ( type == MUS_NONE )
	{
		VORTEK_ERROR( "Failed to add music [{}] from memory. Unable to determine music type.", musicName );
		return false;
	}

	auto pMusic = Mix_LoadMUSType_RW( rw, type, 1 );
	if ( !pMusic )
	{
		VORTEK_ERROR( "Failed to add music [{}] from memory.", musicName );
		return false;
	}

	// Create the sound parameters
	Vortek::Sounds::SoundParams params{
		.name = musicName, .filename = "From Data", .duration = Mix_MusicDuration( pMusic ) };

	// Create the music Pointer
	auto pMusicPtr = std::make_shared<Vortek::Sounds::Music>( params, MusicPtr{ pMusic } );
	if ( !pMusicPtr )
	{
		VORTEK_ERROR( "Failed to create the music ptr for [{}]", musicName );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapMusic.emplace( musicName, std::move( pMusicPtr ) );

	return bSuccess;
}

std::shared_ptr<Vortek::Sounds::Music> AssetManager::GetMusic( const std::string& musicName )
{
	auto musicItr = m_mapMusic.find( musicName );
	if ( musicItr == m_mapMusic.end() )
	{
		VORTEK_ERROR( "Failed to get [{}] -- Does not exist!", musicName );
		return nullptr;
	}

	return musicItr->second;
}

Mix_MusicType AssetManager::DetectAudioFormat( const unsigned char* audioData, size_t dataSize )
{
	if ( !audioData || dataSize < 12 )
	{
		VORTEK_ERROR( "Failed to detect the audio format. Data or size is invalid." );
		return MUS_NONE;
	}

	// WAV Format
	if ( std::memcmp( audioData, "RIFF", 4 ) == 0 && std::memcmp( audioData + 8, "WAVE", 4 ) == 0 )
	{
		return MUS_WAV;
	}

	// MP3 Format
	if ( std::memcmp( audioData, "ID3", 3 ) == 0 || audioData[ 0 ] == 0xFF && ( audioData[ 1 ] & 0xE0 ) == 0xE0 )
	{
		return MUS_MP3;
	}

	// OGG Format
	if ( std::memcmp( audioData, "OggS", 4 ) == 0 )
	{
		return MUS_OGG;
	}

	// Flac Format
	if ( std::memcmp( audioData, "fLaC", 4 ) == 0 )
	{
		return MUS_FLAC;
	}

	if ( dataSize >= 36 && std::memcmp( audioData + 28, "OpusHead", 8 ) == 0 )
	{
		return MUS_OPUS;
	}

	VORTEK_ERROR( "Failed to detect audio type - Unknown or unsupported format." );

	return MUS_NONE;
}

bool AssetManager::AddSoundFx( const std::string& soundFxName, const std::string& filepath )
{
	if ( m_mapSoundFx.contains( soundFxName ) )
	{
		VORTEK_ERROR( "Failed to add soundfx [{}] -- Already exists!", soundFxName );
		return false;
	}

	Mix_Chunk* pChunk = Mix_LoadWAV( filepath.c_str() );

	if ( !pChunk )
	{
		std::string error{ Mix_GetError() };
		VORTEK_ERROR( "Failed to load [{}] at path [{}] -- Error: {}", soundFxName, filepath, error );
		return false;
	}

	Vortek::Sounds::SoundParams params{ .name = soundFxName, .filename = filepath, .duration = pChunk->alen / 179.4 };

	auto pSoundFx = std::make_shared<Vortek::Sounds::SoundFX>( params, SoundFxPtr{ pChunk } );
	auto [ itr, bSuccess ] = m_mapSoundFx.emplace( soundFxName, std::move( pSoundFx ) );

	if ( bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };
		fs::path path{ filepath };
		auto lastWrite = fs::last_write_time( path );
		if ( Vortek::Utilities::CheckContainsValue( m_FilewatchParams,
											 [ & ]( const auto& params ) { return params.sFilepath == filepath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = soundFxName,
															  .sFilepath = filepath,
															  .lastWrite = lastWrite,
															  .eType = Vortek::Utilities::AssetType::SOUNDFX } );
		}
	}

	return bSuccess;
}

bool AssetManager::AddSoundFxFromMemory( const std::string& soundFxName, const unsigned char* soundFxData,
										 size_t dataSize )
{
	if ( m_mapSoundFx.contains( soundFxName ) )
	{
		VORTEK_ERROR( "Failed to add soundfx [{}] -- Already exists!", soundFxName );
		return false;
	}

	SDL_RWops* rw = SDL_RWFromMem( (void*)soundFxData, static_cast<int>( dataSize ) );
	auto pChunk = Mix_LoadWAV_RW( rw, 1 );
	if ( !pChunk )
	{
		VORTEK_ERROR( "Failed to add soundfx [{}] from memory.", soundFxName );
		return false;
	}

	Vortek::Sounds::SoundParams params{ .name = soundFxName, .filename = "From Data", .duration = pChunk->alen / 179.4 };

	auto pSoundFx = std::make_shared<Vortek::Sounds::SoundFX>( params, SoundFxPtr{ pChunk } );
	auto [ itr, bSuccess ] = m_mapSoundFx.emplace( soundFxName, std::move( pSoundFx ) );

	return bSuccess;
}

std::string AssetManager::GetAssetFilepath( const std::string& sAssetName, Vortek::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE: {
		auto itr = m_mapTextures.find( sAssetName );
		return itr != m_mapTextures.end() ? itr->second->GetPath() : std::string{};
	}
	case Vortek::Utilities::AssetType::FONT: {
		auto itr = m_mapFonts.find( sAssetName );
		return itr != m_mapFonts.end() ? itr->second->GetFilename() : std::string{};
	}
	case Vortek::Utilities::AssetType::SOUNDFX: {
		auto itr = m_mapSoundFx.find( sAssetName );
		return itr != m_mapSoundFx.end() ? itr->second->GetFilename() : std::string{};
	}
	case Vortek::Utilities::AssetType::MUSIC: {
		auto itr = m_mapMusic.find( sAssetName );
		return itr != m_mapMusic.end() ? itr->second->GetFilename() : std::string{};
	}
	case Vortek::Utilities::AssetType::PREFAB: {
		auto itr = m_mapPrefabs.find( sAssetName );
		return itr != m_mapPrefabs.end() ? itr->second->GetFilepath() : std::string{};
	}
	}

	return {};
}

std::shared_ptr<Vortek::Sounds::SoundFX> AssetManager::GetSoundFx( const std::string& soundFxName )
{
	auto soundItr = m_mapSoundFx.find( soundFxName );
	if ( soundItr == m_mapSoundFx.end() )
	{
		VORTEK_ERROR( "Failed to get SoundFX [{}] -- Does Not exist!", soundFxName );
		return nullptr;
	}

	return soundItr->second;
}

bool AssetManager::AddPrefab( const std::string& sPrefabName, std::shared_ptr<Vortek::Core::Prefab> pPrefab )
{
	if ( m_mapPrefabs.contains( sPrefabName ) )
	{
		VORTEK_ERROR( "Failed to add prefab [{}] -- Already exists in AssetManager.", sPrefabName );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapPrefabs.emplace( sPrefabName, std::move( pPrefab ) );
	return bSuccess;
}

std::shared_ptr<Vortek::Core::Prefab> AssetManager::GetPrefab( const std::string& sPrefabName )
{
	auto prefabItr = m_mapPrefabs.find( sPrefabName );
	if ( prefabItr == m_mapPrefabs.end() )
	{
		VORTEK_ERROR( "Failed to get Prefab [{}] -- Does Not exist!", sPrefabName );
		return nullptr;
	}

	return prefabItr->second;
}

#ifdef IN_VORTEK_EDITOR

bool AssetManager::AddCursor( const std::string& sCursorName, const std::string& sCursorPath )
{
	return false;
}

bool AssetManager::AddCursorFromMemory( const std::string& sCursorName, unsigned char* cursorData, size_t dataSize )
{
	if ( m_mapCursors.contains( sCursorName ) )
	{
		VORTEK_ERROR( "Failed to add Cursor [{}] - Already exists.", sCursorName );
		return false;
	}

	SDL_RWops* rw = SDL_RWFromConstMem( cursorData, static_cast<int>( dataSize ) );
	if ( !rw )
	{
		VORTEK_ERROR( "Failed to add cursor. [{}]", SDL_GetError() );
		return false;
	}

	SDL_Surface* pSurface = IMG_Load_RW( rw, 1 ); // 1 = Automatically closes RWops
	if ( !pSurface )
	{
		VORTEK_ERROR( "Failed to add cursor. [{}]", IMG_GetError() );
		return false;
	}

	SDL_Cursor* pCursor = SDL_CreateColorCursor( pSurface, pSurface->w / 2, pSurface->h / 2 );

	if ( !pCursor )
	{
		VORTEK_ERROR( "Failed to add cursor. [{}]", SDL_GetError() );
		return false;
	}

	SDL_FreeSurface( pSurface );

	return m_mapCursors.emplace( sCursorName, MakeSharedFromSDLType<Cursor>( pCursor ) ).second;
}

SDL_Cursor* AssetManager::GetCursor( const std::string& sCursorName )
{
	auto cursorItr = m_mapCursors.find( sCursorName );
	if ( cursorItr == m_mapCursors.end() )
	{
		VORTEK_ERROR( "Failed to get Cursor [{}] -- Does Not exist!", sCursorName );
		return nullptr;
	}

	return cursorItr->second.get();
}

#endif

std::vector<std::string> AssetManager::GetAssetKeyNames( Vortek::Utilities::AssetType eAssetType ) const
{
	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE:
		return Vortek::Utilities::GetKeys( m_mapTextures, []( const auto& pair ) { return !pair.second->IsEditorTexture(); } );
	case Vortek::Utilities::AssetType::FONT: return Vortek::Utilities::GetKeys( m_mapFonts );
	case Vortek::Utilities::AssetType::SOUNDFX: return Vortek::Utilities::GetKeys( m_mapSoundFx );
	case Vortek::Utilities::AssetType::MUSIC: return Vortek::Utilities::GetKeys( m_mapMusic );
	case Vortek::Utilities::AssetType::PREFAB: return Vortek::Utilities::GetKeys( m_mapPrefabs );
	default: VORTEK_ASSERT( false && "Cannot get this type!" );
	}

	return std::vector<std::string>{};
}

bool AssetManager::ChangeAssetName( const std::string& sOldName, const std::string& sNewName,
									Vortek::Utilities::AssetType eAssetType )
{
	bool bSuccess{ false };

	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE: bSuccess = Vortek::Utilities::KeyChange( m_mapTextures, sOldName, sNewName ); break;
	case Vortek::Utilities::AssetType::FONT: bSuccess = Vortek::Utilities::KeyChange( m_mapFonts, sOldName, sNewName ); break;
	case Vortek::Utilities::AssetType::SOUNDFX: bSuccess = Vortek::Utilities::KeyChange( m_mapSoundFx, sOldName, sNewName ); break;
	case Vortek::Utilities::AssetType::MUSIC: bSuccess = Vortek::Utilities::KeyChange( m_mapMusic, sOldName, sNewName ); break;
	default: VORTEK_ASSERT( false && "Cannot get this type!" ); break;
	}

	// If we are using the filewatcher, we need to also ensure to adjust the name
	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };
		auto fileItr = std::ranges::find_if( m_FilewatchParams,
											 [ & ]( const auto& param ) { return param.sAssetName == sOldName; } );

		if ( fileItr != m_FilewatchParams.end() )
		{
			fileItr->sAssetName = sNewName;
		}
	}

	return bSuccess;
}

bool AssetManager::CheckHasAsset( const std::string& sNameCheck, Vortek::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE: return m_mapTextures.contains( sNameCheck );
	case Vortek::Utilities::AssetType::FONT: return m_mapFonts.contains( sNameCheck );
	case Vortek::Utilities::AssetType::SOUNDFX: return m_mapSoundFx.contains( sNameCheck );
	case Vortek::Utilities::AssetType::MUSIC: return m_mapMusic.contains( sNameCheck );
	case Vortek::Utilities::AssetType::PREFAB: return m_mapPrefabs.contains( sNameCheck );
	default: VORTEK_ASSERT( false && "Cannot get this type!" );
	}

	return false;
}

bool AssetManager::DeleteAsset( const std::string& sAssetName, Vortek::Utilities::AssetType eAssetType )
{
	bool bSuccess{ false };

	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE:
		bSuccess = std::erase_if( m_mapTextures, [ & ]( const auto& pair ) { return pair.first == sAssetName; } ) > 0;
		break;
	case Vortek::Utilities::AssetType::FONT:
		bSuccess = std::erase_if( m_mapFonts, [ & ]( const auto& pair ) { return pair.first == sAssetName; } ) > 0;
		break;
	case Vortek::Utilities::AssetType::SOUNDFX:
		bSuccess = std::erase_if( m_mapSoundFx, [ & ]( const auto& pair ) { return pair.first == sAssetName; } ) > 0;
		break;
	case Vortek::Utilities::AssetType::MUSIC:
		bSuccess = std::erase_if( m_mapMusic, [ & ]( const auto& pair ) { return pair.first == sAssetName; } ) > 0;
		break;
	case Vortek::Utilities::AssetType::PREFAB: { // Prefabs contain files that must be cleaned up
		if ( auto pPrefab = GetPrefab( sAssetName ) )
		{
			if ( !Vortek::Core::PrefabCreator::DeletePrefab( *pPrefab ) )
			{
				VORTEK_ERROR( "Failed to delete prefab [{}]", sAssetName );
				return false;
			}

			bSuccess = m_mapPrefabs.erase( sAssetName ) > 0;
			break;
		}

		VORTEK_ERROR( "Failed to delete prefab [{}] - Does not exist in asset manager.", sAssetName );
		return false;
	}
	default: VORTEK_ASSERT( false && "Cannot get this type!" );
	}

	// If the file watcher is enabled, we need to remove the
	// file from being watched.
	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };
		bool bErased = std::erase_if( m_FilewatchParams,
									  [ & ]( const auto& param ) { return param.sAssetName == sAssetName; } ) > 0;

		if ( !bErased )
		{
			VORTEK_WARN( "Failed to erase [{}] from File Watcher Params. - Must not be present.", sAssetName );
			// Non-fatal error.
		}
	}

	if ( bSuccess )
	{
		VORTEK_LOG( "Deleted asset [{}]", sAssetName );
	}

	return bSuccess;
}

bool AssetManager::DeleteAssetFromPath( const std::string& sAssetPath )
{
	auto textureItr = std::ranges::find_if(
		m_mapTextures, [ & ]( const auto& pair ) { return pair.second->GetPath() == sAssetPath; } );

	if ( textureItr != m_mapTextures.end() )
	{
		std::string sTextureName{ textureItr->first };
		return DeleteAsset( sTextureName, Vortek::Utilities::AssetType::TEXTURE );
	}

	auto musicItr = std::ranges::find_if(
		m_mapMusic, [ & ]( const auto& pair ) { return pair.second->GetFilename() == sAssetPath; } );

	if ( musicItr != m_mapMusic.end() )
	{
		std::string sMusicName{ musicItr->first };
		return DeleteAsset( sMusicName, Vortek::Utilities::AssetType::MUSIC );
	}

	auto soundItr = std::ranges::find_if(
		m_mapSoundFx, [ & ]( const auto& pair ) { return pair.second->GetFilename() == sAssetPath; } );

	if ( soundItr != m_mapSoundFx.end() )
	{
		std::string sSoundName{ soundItr->first };
		return DeleteAsset( sSoundName, Vortek::Utilities::AssetType::SOUNDFX );
	}

	return true;
}

void AssetManager::CreateLuaAssetManager( sol::state& lua )
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& asset_manager = mainRegistry.GetAssetManager();

	lua.new_usertype<AssetManager>(
		"AssetManager",
		sol::no_constructor,
		"addTexture",
		sol::overload(
			[ & ]( const std::string& assetName, const std::string& filepath, bool pixel_art ) {
				return asset_manager.AddTexture( assetName, filepath, pixel_art, false );
			},
			[ & ]( const std::string& assetName, const std::string& filepath, bool pixel_art, bool bTileset ) {
				return asset_manager.AddTexture( assetName, filepath, pixel_art, bTileset );
			} ),
		"addMusic",
		[ & ]( const std::string& musicName, const std::string& filepath ) {
			return asset_manager.AddMusic( musicName, filepath );
		},
		"addSoundfx",
		[ & ]( const std::string& soundFxName, const std::string& filepath ) {
			return asset_manager.AddSoundFx( soundFxName, filepath );
		},
		"addFont",
		[ & ]( const std::string& fontName, const std::string& fontPath, float fontSize ) {
			return asset_manager.AddFont( fontName, fontPath, fontSize );
		} );
}
void AssetManager::Update()
{
	std::shared_lock sharedLock{ m_AssetMutex };
	auto dirtyView = m_FilewatchParams | std::views::filter( []( const auto& param ) { return param.bDirty; } );

	if ( !dirtyView.empty() )
	{
		sharedLock.unlock();
		std::unique_lock lock{ m_AssetMutex };
		for ( auto& param : dirtyView )
		{
			ReloadAsset( param );
			param.bDirty = false;
		}
	}
}

void AssetManager::FileWatcher()
{
	while ( m_bFileWatcherRunning )
	{
		std::this_thread::sleep_for( 2s );

		for ( auto& fileParam : m_FilewatchParams )
		{
			std::shared_lock sharedLock{ m_AssetMutex };
			fs::path path{ fileParam.sFilepath };
			if ( !fs::exists( path ) )
				continue;

			if ( fileParam.lastWrite != fs::last_write_time( path ) )
			{
				sharedLock.unlock();
				std::unique_lock lock{ m_AssetMutex };
				fileParam.bDirty = true;
			}
		}
	}
}

void AssetManager::ReloadAsset( const AssetWatchParams& assetParams )
{
	switch ( assetParams.eType )
	{
	case Vortek::Utilities::AssetType::TEXTURE: ReloadTexture( assetParams.sAssetName ); break;
	case Vortek::Utilities::AssetType::FONT: ReloadFont( assetParams.sAssetName ); break;
	case Vortek::Utilities::AssetType::SHADER: ReloadShader( assetParams.sAssetName ); break;
	case Vortek::Utilities::AssetType::MUSIC: ReloadMusic( assetParams.sAssetName ); break;
	case Vortek::Utilities::AssetType::SOUNDFX: ReloadSoundFx( assetParams.sAssetName ); break;
	}
}

void AssetManager::ReloadTexture( const std::string& sTextureName )
{
	auto fileParamItr = std::ranges::find_if( m_FilewatchParams,
											  [ & ]( const auto& param ) { return param.sAssetName == sTextureName; } );

	if ( fileParamItr == m_FilewatchParams.end() )
	{
		VORTEK_ERROR( "Trying to reload a texture that has not been loaded?" );
		return;
	}

	// We are assuming that the texture is in the map.
	// Could potentially cause a crash, will look more into this.
	auto& pTexture = m_mapTextures[ sTextureName ];

	fileParamItr->lastWrite = fs::last_write_time( fs::path{ pTexture->GetPath() } );
	// Delete the old texture and then reload
	auto id = pTexture->GetID();
	glDeleteTextures( 1, &id );

	auto pNewTexture =
		Vortek::Rendering::TextureLoader::Create( pTexture->GetType(), pTexture->GetPath(), pTexture->IsTileset() );

	pTexture = pNewTexture;
	VORTEK_LOG( "Reloaded texture: {}", sTextureName );
}

void AssetManager::ReloadSoundFx( const std::string& sSoundFxName )
{
	auto fileParamItr = std::ranges::find_if( m_FilewatchParams,
											  [ & ]( const auto& param ) { return param.sAssetName == sSoundFxName; } );

	if ( fileParamItr == m_FilewatchParams.end() )
	{
		VORTEK_ERROR( "Trying to reload a texture that has not been loaded?" );
		return;
	}

	fileParamItr->lastWrite = fs::last_write_time( fs::path{ fileParamItr->sFilepath } );

	if ( !DeleteAsset( sSoundFxName, Vortek::Utilities::AssetType::SOUNDFX ) )
	{
		VORTEK_ERROR( "Failed to Reload SoundFx: {}", sSoundFxName );
		return;
	}

	if ( !AddSoundFx( sSoundFxName, fileParamItr->sFilepath ) )
	{
		VORTEK_ERROR( "Failed to Reload SoundFx: {}", sSoundFxName );
		return;
	}

	VORTEK_LOG( "Reloaded SoundFx: {}", sSoundFxName );
}

void AssetManager::ReloadMusic( const std::string& sMusicName )
{
	auto fileParamItr = std::ranges::find_if( m_FilewatchParams,
											  [ & ]( const auto& param ) { return param.sAssetName == sMusicName; } );

	if ( fileParamItr == m_FilewatchParams.end() )
	{
		VORTEK_ERROR( "Trying to music that has not been loaded?" );
		return;
	}

	fileParamItr->lastWrite = fs::last_write_time( fs::path{ fileParamItr->sFilepath } );

	if ( !DeleteAsset( sMusicName, Vortek::Utilities::AssetType::MUSIC ) )
	{
		VORTEK_ERROR( "Failed to Reload SoundFx: {}", sMusicName );
		return;
	}

	if ( !AddMusic( sMusicName, fileParamItr->sFilepath ) )
	{
		VORTEK_ERROR( "Failed to Reload SoundFx: {}", sMusicName );
		return;
	}

	VORTEK_LOG( "Reloaded Music: {}", sMusicName );
}

void AssetManager::ReloadFont( const std::string& sFontName )
{
	auto fileParamItr =
		std::ranges::find_if( m_FilewatchParams, [ & ]( const auto& param ) { return param.sAssetName == sFontName; } );

	if ( fileParamItr == m_FilewatchParams.end() )
	{
		VORTEK_ERROR( "Trying to music that has not been loaded?" );
		return;
	}

	fileParamItr->lastWrite = fs::last_write_time( fs::path{ fileParamItr->sFilepath } );

	auto& pFont = m_mapFonts[ sFontName ];
	float fontSize = pFont->GetFontSize();

	if ( !DeleteAsset( sFontName, Vortek::Utilities::AssetType::FONT ) )
	{
		VORTEK_ERROR( "Failed to Reload SoundFx: {}", sFontName );
		return;
	}

	if ( !AddFont( sFontName, fileParamItr->sFilepath, fontSize ) )
	{
		VORTEK_ERROR( "Failed to Reload SoundFx: {}", sFontName );
		return;
	}

	VORTEK_LOG( "Reloaded Font: {}", sFontName );
}

void AssetManager::ReloadShader( const std::string& sShaderName )
{
	// TODO:
}

} // namespace VORTEK_RESOURCES
