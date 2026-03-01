#pragma once

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Editor
{
struct Tile;

struct RectToolAddTilesCmd
{
	Vortek::Core::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

struct RectToolRemoveTilesCmd
{
	Vortek::Core::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

} // namespace Vortek::Editor
