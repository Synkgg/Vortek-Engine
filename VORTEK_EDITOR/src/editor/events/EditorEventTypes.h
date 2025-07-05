#pragma once
#include <string>

namespace VORTEK_CORE::ECS
{
class Entity;
}

namespace VORTEK_EDITOR::Events
{
enum class EFileAction
{
	Delete,
	Copy,
	Cut,
	Paste,
	Rename,
	FileDropped,
	NoAction
};

struct FileEvent
{
	EFileAction eAction{ EFileAction::NoAction };
	std::string sFilepath{};
};

struct CloseEditorEvent
{
	//
};

struct SwitchEntityEvent
{
	VORTEK_CORE::ECS::Entity* pEntity{ nullptr };
};

enum class EContentCreateAction
{
	Folder,
	LuaClass,
	LuaTable,
	EmptyLuaFile,

	NoAction
};

struct ContentCreateEvent
{
	EContentCreateAction eAction{ EContentCreateAction::NoAction };
	std::string sFilepath{};
};

struct NameChangeEvent
{
	std::string sOldName{};
	std::string sNewName{};
	VORTEK_CORE::ECS::Entity* pEntity{ nullptr };
};

enum class EComponentType
{
	Transform,
	Sprite,
	Physics,
	Text,
	BoxCollider,
	CircleCollider,
	RigidBody,
	Animation,
	Tile,
	UI,
	NoType
};

struct AddComponentEvent
{
	VORTEK_CORE::ECS::Entity* pEntity{ nullptr };
	EComponentType eType{ EComponentType::NoType };
};

EComponentType GetComponentTypeFromStr( const std::string& componentStr );
std::string GetComponentStrFromType( EComponentType eType );

} // namespace VORTEK_EDITOR::Events
