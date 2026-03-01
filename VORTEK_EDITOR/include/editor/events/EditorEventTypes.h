#pragma once

namespace Vortek::Core::ECS
{
class Entity;
}

namespace Vortek::Editor::Events
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
	Vortek::Core::ECS::Entity* pEntity{ nullptr };
};

enum class EContentCreateAction
{
	/* Create a new folder. */
	Folder,
	/* Create a new lua file with Class */
	LuaClass,
	/* Create a new lua file with an empty lua table. TableName = {} */
	LuaTable,
	/* Create a new lua file with Class that has a Vortek State class implementation. */
	LuaStateClass,
	/* Create an empty lua file.*/
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
	Vortek::Core::ECS::Entity* pEntity{ nullptr };
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
	Vortek::Core::ECS::Entity* pEntity{ nullptr };
	EComponentType eType{ EComponentType::NoType };
};

EComponentType GetComponentTypeFromStr( const std::string& componentStr );
std::string GetComponentStrFromType( EComponentType eType );

} // namespace Vortek::Editor::Events
