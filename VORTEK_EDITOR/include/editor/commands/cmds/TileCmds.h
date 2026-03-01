#pragma once
#include "VortekUtilities/HelperUtilities.h"

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Editor
{
struct Tile;
class SceneObject;

struct RemoveTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	std::vector<Tile> tilesRemoved;
	Vortek::Utilities::SpriteLayerParams spriteLayerParams;

	void undo();
	void redo();
};

struct AddTileLayerCmd
{
	SceneObject* pSceneObject{ nullptr };
	Vortek::Utilities::SpriteLayerParams spriteLayerParams;

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

} // namespace Vortek::Editor
