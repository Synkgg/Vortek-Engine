#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include "Physics/UserData.h"
#include <filesystem>

namespace VORTEK_RENDERING
{
	class Texture;
}

namespace VORTEK_CORE
{
class ProjectInfo;
}

namespace VORTEK_EDITOR
{
constexpr const std::string_view DROP_TEXTURE_SRC = "DropTextureSource";
constexpr const std::string_view DROP_FONT_SRC = "DropFontSource";
constexpr const std::string_view DROP_SOUNDFX_SRC = "DropSoundFxSource";
constexpr const std::string_view DROP_MUSIC_SRC = "DropMusicSource";
constexpr const std::string_view DROP_SCENE_SRC = "DropSceneSource";
constexpr const std::string_view DROP_PREFAB_SRC = "DropPrefabSource";

	struct Tile
	{
		uint32_t id{ entt::null };
		VORTEK_CORE::ECS::TransformComponent transform{};
		VORTEK_CORE::ECS::SpriteComponent sprite{};
		VORTEK_CORE::ECS::AnimationComponent animation{};
		VORTEK_CORE::ECS::BoxColliderComponent boxCollider{};
		VORTEK_CORE::ECS::CircleColliderComponent circleCollider{};
		VORTEK_CORE::ECS::PhysicsComponent physics{};
		VORTEK_PHYSICS::ObjectData objectData{};

		bool bCollider{ false }, bAnimation{ false }, bPhysics{ false }, bCircle{ false };
	};

	constexpr VORTEK_RENDERING::Color XAXIS_GIZMO_COLOR = { 255, 0, 0, 175 };
	constexpr VORTEK_RENDERING::Color XAXIS_HOVERED_GIZMO_COLOR = { 255, 255, 0, 175 };
	constexpr VORTEK_RENDERING::Color YAXIS_GIZMO_COLOR = { 0, 255, 0, 175 };
	constexpr VORTEK_RENDERING::Color YAXIS_HOVERED_GIZMO_COLOR = { 0, 255, 255, 175 };

	constexpr VORTEK_RENDERING::Color GRAYED_OUT_GIZMO_COLOR = { 135, 135, 135, 175 };

	struct MouseGuiInfo
	{
		glm::vec2 position{ 0.f };
		glm::vec2 windowSize{ 0.f };
	};

	struct GizmoAxisParams
	{
		VORTEK_CORE::ECS::TransformComponent transform{};
		VORTEK_CORE::ECS::SpriteComponent sprite{};
		glm::vec2 axisOffset;

		VORTEK_RENDERING::Color axisColor;
		VORTEK_RENDERING::Color axisHoveredColor;
		VORTEK_RENDERING::Color axisDisabledColor;
	};

	/* Supported file types */
	enum class EFileType
	{
		SOUND,
		IMAGE,
		TXT, // All text file types
		FOLDER,
		INVALID_TYPE,
	};

	EFileType GetFileType(const std::string& sPath);

	std::vector<std::string> SplitStr(const std::string& str, char delimiter);
	VORTEK_RENDERING::Texture* GetIconTexture(const std::string& sPath);
	bool IsReservedPathOrFile( const std::filesystem::path& path );
	bool IsDefaultProjectPathOrFile( const std::filesystem::path& path, const VORTEK_CORE::ProjectInfo& projectInfo );

} // namespace VORTEK_EDITOR
