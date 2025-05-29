-- Main Lua Script!
RunScript("assets/scripts/platformer/script_list.lua")

LoadScriptTable(ScriptList)

LoadAssets(AssetDefs)
log("Reloaded")

--[[

gFollowCam = nil 
gCollisionEvent = nil 
gTriggerSystem = nil 
gPlayer = nil 
gMainCam = Camera.get()
gMainCam.set_scale(2.0)

gStateStack = StateStack()
local title = TitleState:Create(gStateStack)
gStateStack:change_state(title)
--]]

log("Running new change!")

main = {
	[1] = {
		update = function()
			if gStateStack then
				gStateStack:update(0.016)
			end
		end
	},
	[2] = {
		render = function()
			if gStateStack then
				gStateStack:render()
			end
		end
	},
}