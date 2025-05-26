#pragma once
#include <memory>

namespace VORTEK_CORE::ECS
{
	class Registry;
}

namespace VORTEK_EDITOR
{
	struct Tile;

	struct CreateTileToolAddCmd
	{
		VORTEK_CORE::ECS::Registry* pRegistry{ nullptr };
		std::shared_ptr<Tile> pTile{ nullptr };

		void undo();
		void redo();
	};

	struct CreateTileToolRemoveCmd
	{
		VORTEK_CORE::ECS::Registry* pRegistry{ nullptr };
		std::shared_ptr<Tile> pTile{ nullptr };

		void undo();
		void redo();
	};

}