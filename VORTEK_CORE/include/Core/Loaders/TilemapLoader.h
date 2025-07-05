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
  public:
	TilemapLoader() = default;
	~TilemapLoader() = default;

	bool SaveTilemap( VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );
	bool LoadTilemap( VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );
	bool LoadGameObjects( VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile, bool bUseJSON = false );
	bool SaveGameObjects( VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile, bool bUseJSON = false );

  private:
	bool SaveTilemapJSON( VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile );
	bool LoadTilemapJSON( VORTEK_CORE::ECS::Registry& registry, const std::string& sTilemapFile );

	bool SaveObjectMapJSON( VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );
	bool LoadObjectMapJSON( VORTEK_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );

	// TODO: Save and load functions for lua serializer
};

/*
 * We cannot just save the entity IDs for relationships since they are not guaranteed to be the same
 * by entt; however, we can try to guarantee for each scene that the entity has a unique tag.
 */
struct SaveRelationship
{
	// entt::entity self{ entt::null };
	std::string sParent{};
	std::string sNextSibling{};
	std::string sPrevSibling{};
	std::string sFirstChild{};
};

} // namespace VORTEK_CORE::Loaders
