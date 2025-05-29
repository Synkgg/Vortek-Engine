#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include "Physics/UserData.h"

namespace VORTEK_RENDERING
{
	class Texture;
}

namespace VORTEK_EDITOR
{
	constexpr const char* DROP_TEXTURE_SRC = "DropTextureSource";
	constexpr const char* DROP_FONT_SRC = "DropFontSource";
	constexpr const char* DROP_SOUNDFX_SRC = "DropSoundFxSource";
	constexpr const char* DROP_MUSIC_SRC = "DropMusicSource";
	constexpr const char* DROP_SCENE_SRC = "DropSceneSource";

#define BASE_PATH                                                                                                      \
	std::string                                                                                                        \
	{                                                                                                                  \
		SDL_GetBasePath()                                                                                              \
	}

#ifdef _WIN32
	constexpr char PATH_SEPARATOR = '\\';
#define DEFAULT_PROJECT_PATH BASE_PATH + "VortekProjects"
#else
	constexpr char PATH_SEPARATOR = '/';
#define DEFAULT_PROJECT_PATH BASE_PATH + PATH_SEPARATOR + "VortekProjects"
#endif

#define SCRIPTS "scripts"
#define ASSETS "assets" 

	constexpr const char* PRJ_FILE_EXT = ".veprj";

	struct Canvas
	{
		int width{ 640 }, height{ 480 }, tileWidth{ 16 }, tileHeight{ 16 };
	};

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

} // namespace VORTEK_EDITOR