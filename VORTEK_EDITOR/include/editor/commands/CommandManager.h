#pragma once
#include "AbstractCommandManager.h"
#include "cmds/CreateTileToolCmds.h"
#include "cmds/RectToolCmds.h"
#include "cmds/TileCmds.h"
// TODO: add other commands as necessary

namespace Vortek::Editor
{
using UndoableCommands = VarCommands<CreateTileToolAddCmd, CreateTileToolRemoveCmd, RectToolAddTilesCmd,
									 RectToolRemoveTilesCmd, RemoveTileLayerCmd, AddTileLayerCmd,
									 ChangeTileLayerNameCmd, MoveTileLayerCmd /* Add other command types */>;

class CommandManager
	: public AbstractCommandManager<CreateTileToolAddCmd, CreateTileToolRemoveCmd, RectToolAddTilesCmd,
									RectToolRemoveTilesCmd, RemoveTileLayerCmd, AddTileLayerCmd, ChangeTileLayerNameCmd,
									MoveTileLayerCmd /* Add other command types */>
{
};

} // namespace Vortek::Editor
