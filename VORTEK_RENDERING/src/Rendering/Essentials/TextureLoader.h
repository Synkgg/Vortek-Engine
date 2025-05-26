#pragma once
#include "Texture.h"
#include <memory>

namespace VORTEK_RENDERING {
	class TextureLoader
	{
	private:
		static bool LoadTexture(const std::string& filepath, GLuint& id, int& width, int& height, bool blended = false);
		static bool LoadFBTexture(GLuint& id, int& width, int& height);
		static bool LoadTextureFromMemory(const unsigned char* imageData, size_t length, GLuint& id, int& width, int& height, bool blended = false);

	public:
		TextureLoader() = delete;

		static std::shared_ptr<Texture> Create(Texture::TextureType type, const std::string& texturePath, bool bTileset = false);
		static std::shared_ptr<Texture> Create(Texture::TextureType type, int width, int height, bool bTileset = false);

		static std::shared_ptr<Texture> CreateFromMemory(const unsigned char* imageData, size_t length, bool blended = false, bool bTileset = false);
	};
}