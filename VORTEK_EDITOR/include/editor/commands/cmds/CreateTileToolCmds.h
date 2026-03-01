#pragma once

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Editor
{
struct Tile;

struct CreateTileToolAddCmd
{
	Vortek::Core::ECS::Registry* pRegistry{ nullptr };
	std::shared_ptr<Tile> pTile{ nullptr };

	void undo();
	void redo();
};

struct CreateTileToolRemoveCmd
{
	Vortek::Core::ECS::Registry* pRegistry{ nullptr };
	std::shared_ptr<Tile> pTile{ nullptr };

	void undo();
	void redo();
};

} // namespace Vortek::Editor
