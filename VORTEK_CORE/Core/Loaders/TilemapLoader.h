#pragma once
#include <string>

namespace VORTEK_CORE::ECS
{
	class Registry;
}

namespace VORTEK_CORE::Loaders
{
	class TilemapLoader
	{
	private:
		bool SaveTilemapJSON(VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile);
		bool LoadTilemapJSON(VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile);

		// TODO: Save and load functions for lua serializer

	public:
		TilemapLoader() = default;
		~TilemapLoader() = default;

		bool SaveTilemap(VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false);
		bool LoadTilemap(VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false);
	};
} // namespace VORTEK_CORE::Loaders