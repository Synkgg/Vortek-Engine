#pragma once
#include "VortekUtilities/HelperUtilities.h"

namespace VORTEK_CORE::ECS
{
class Registry;
}

namespace VORTEK_EDITOR
{
struct Tile;
class SceneObject;

struct RemoveTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::vector<Tile> tilesRemoved;
	VORTEK_UTIL::SpriteLayerParams spriteLayerParams;

	void undo();
	void redo();
};

struct AddTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	VORTEK_UTIL::SpriteLayerParams spriteLayerParams;

	void undo();
	void redo();
};

struct MoveTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	int from;
	int to;

	void undo();
	void redo();
};

struct ChangeTileLayerNameCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::string sOldName{};
	std::string sNewName{};

	void undo();
	void redo();
};

} // namespace VORTEK_EDITOR
