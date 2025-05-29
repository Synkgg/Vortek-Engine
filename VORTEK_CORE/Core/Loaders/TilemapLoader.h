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

		bool SaveObjectMapJSON(VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile);
		bool LoadObjectMapJSON(VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile);

		// TODO: Save and load functions for lua serializer

	public:
		TilemapLoader() = default;
		~TilemapLoader() = default;

		bool SaveTilemap(VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false);
		bool LoadTilemap(VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false);
		bool LoadGameObjects(VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile, bool bUseJSON = false);
		bool SaveGameObjects(VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile, bool bUseJSON = false);
	};
} // namespace VORTEK_CORE::Loaders