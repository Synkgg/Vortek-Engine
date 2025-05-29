#pragma once
#include <map>
#include <memory>
#include <string>

#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Essentials/Font.h>
#include <Sounds/Essentials/Music.h>
#include <Sounds/Essentials/SoundFX.h>

#include "../ECS/Registry.h"
#include <sol/sol.hpp>

namespace VORTEK_UTIL
{
	enum class AssetType;
}

namespace VORTEK_RESOURCES
{

	class AssetManager
	{
	private:
		std::map<std::string, std::shared_ptr<VORTEK_RENDERING::Texture>> m_mapTextures{};
		std::map<std::string, std::shared_ptr<VORTEK_RENDERING::Shader>> m_mapShader{};
		std::map<std::string, std::shared_ptr<VORTEK_RENDERING::Font>> m_mapFonts{};

		std::map<std::string, std::shared_ptr<VORTEK_SOUNDS::Music>> m_mapMusic{};
		std::map<std::string, std::shared_ptr<VORTEK_SOUNDS::SoundFX>> m_mapSoundFx{};

	public:
		AssetManager() = default;
		~AssetManager() = default;

		bool CreateDefaultFonts();

		bool AddTexture(const std::string& textureName, const std::string& texturePath, bool pixelArt = true,
			bool bTileset = false);
		bool AddTextureFromMemory(const std::string& textureName, const unsigned char* imageData, size_t length,
			bool pixelArt = true, bool bTileset = false);

		std::shared_ptr<VORTEK_RENDERING::Texture> GetTexture(const std::string& textureName);

		std::vector<std::string> GetTilesetNames() const;

		bool AddFont(const std::string& fontName, const std::string& fontPath, float fontSize = 32.f);
		bool AddFontFromMemory(const std::string& fontName, unsigned char* fontData, float fontSize = 32.f);
		std::shared_ptr<VORTEK_RENDERING::Font> GetFont(const std::string& fontName);

		bool AddShader(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);
		bool AddShaderFromMemory(const std::string& shaderName, const char* vertexShader, const char* fragmentShader);
		std::shared_ptr<VORTEK_RENDERING::Shader> GetShader(const std::string& shaderName);

		bool AddMusic(const std::string& musicName, const std::string& filepath);
		std::shared_ptr<VORTEK_SOUNDS::Music> GetMusic(const std::string& musicName);

		bool AddSoundFx(const std::string& soundFxName, const std::string& filepath);
		std::shared_ptr<VORTEK_SOUNDS::SoundFX> GetSoundFx(const std::string& soundFxName);

		inline const std::map<std::string, std::shared_ptr<VORTEK_RENDERING::Texture>>& GetAllTextures() const
		{
			return m_mapTextures;
		}

		inline const std::map<std::string, std::shared_ptr<VORTEK_SOUNDS::SoundFX>>& GetAllSoundFx() const
		{
			return m_mapSoundFx;
		}

		inline const std::map<std::string, std::shared_ptr<VORTEK_RENDERING::Shader>>& GetAllShaders() const
		{
			return m_mapShader;
		}

		inline const std::map<std::string, std::shared_ptr<VORTEK_RENDERING::Font>>& GetAllFonts() const
		{
			return m_mapFonts;
		}

		inline const std::map<std::string, std::shared_ptr<VORTEK_SOUNDS::Music>>& GetAllMusic() const
		{
			return m_mapMusic;
		}

		std::vector<std::string> GetAssetKeyNames(VORTEK_UTIL::AssetType eAssetType) const;

		bool ChangeAssetName(const std::string& sOldName, const std::string& sNewName, VORTEK_UTIL::AssetType eAssetType);
		bool CheckHasAsset(const std::string& sNameCheck, VORTEK_UTIL::AssetType eAssetType);
		bool DeleteAsset(const std::string& sAssetName, VORTEK_UTIL::AssetType eAssetType);

		static void CreateLuaAssetManager(sol::state& lua);
	};
} // namespace VORTEK_RESOURCES