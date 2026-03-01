#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include "Physics/UserData.h"

namespace Vortek::Rendering
{
class Texture;
}

namespace Vortek::Core
{
class ProjectInfo;
}

namespace Vortek::Editor
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
	Vortek::Core::ECS::TransformComponent transform{};
	Vortek::Core::ECS::SpriteComponent sprite{};
	Vortek::Core::ECS::AnimationComponent animation{};
	Vortek::Core::ECS::BoxColliderComponent boxCollider{};
	Vortek::Core::ECS::CircleColliderComponent circleCollider{};
	Vortek::Core::ECS::PhysicsComponent physics{};
	Vortek::Physics::ObjectData objectData{};

	bool bCollider{ false }, bAnimation{ false }, bPhysics{ false }, bCircle{ false };
};

constexpr Vortek::Rendering::Color XAXIS_GIZMO_COLOR = { 255, 0, 0, 175 };
constexpr Vortek::Rendering::Color XAXIS_HOVERED_GIZMO_COLOR = { 255, 255, 0, 175 };
constexpr Vortek::Rendering::Color YAXIS_GIZMO_COLOR = { 0, 255, 0, 175 };
constexpr Vortek::Rendering::Color YAXIS_HOVERED_GIZMO_COLOR = { 0, 255, 255, 175 };

constexpr Vortek::Rendering::Color GRAYED_OUT_GIZMO_COLOR = { 135, 135, 135, 175 };

struct MouseGuiInfo
{
	glm::vec2 position{ 0.f };
	glm::vec2 windowSize{ 0.f };
};

struct GizmoAxisParams
{
	Vortek::Core::ECS::TransformComponent transform{};
	Vortek::Core::ECS::SpriteComponent sprite{};
	glm::vec2 axisOffset;

	Vortek::Rendering::Color axisColor;
	Vortek::Rendering::Color axisHoveredColor;
	Vortek::Rendering::Color axisDisabledColor;
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

EFileType GetFileType( const std::string& sPath );

std::vector<std::string> SplitStr( const std::string& str, char delimiter );
Vortek::Rendering::Texture* GetIconTexture( const std::string& sPath );
bool IsReservedPathOrFile( const std::filesystem::path& path );
bool IsDefaultProjectPathOrFile( const std::filesystem::path& path, const Vortek::Core::ProjectInfo& projectInfo );

} // namespace Vortek::Editor
