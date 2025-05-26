script_list = 
{
	"../_Games/asteroids/scripts/utilities.lua",
	"../_Games/asteroids/scripts/game_data.lua",
	"../_Games/asteroids/scripts/assetDefs.lua",
	"../_Games/asteroids/scripts/entityDefs.lua",
	"../_Games/asteroids/scripts/projectile.lua",
	"../_Games/asteroids/scripts/ship.lua",
	"../_Games/asteroids/scripts/asteroid.lua",
	"../_Games/asteroids/scripts/hud.lua",
	"../_Games/asteroids/scripts/collision_system.lua",
}

function LoadScripts()
	for k, v in ipairs(script_list) do
		if not run_script(v) then
			print("Failed to load [" ..v .."]")
		end
	end
end