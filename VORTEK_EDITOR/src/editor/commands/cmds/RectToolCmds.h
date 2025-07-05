#pragma once
#include <vector>

namespace VORTEK_CORE::ECS
{
class Registry;
}

namespace VORTEK_EDITOR
{
struct Tile;

struct RectToolAddTilesCmd
{
	VORTEK_CORE::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

struct RectToolRemoveTilesCmd
{
	VORTEK_CORE::ECS::Registry* pRegistry{ nullptr };
	std::vector<Tile> tiles{};

	void undo();
	void redo();
};

} // namespace VORTEK_EDITOR
