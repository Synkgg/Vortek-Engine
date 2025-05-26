AssetDefs = 
{
	textures =
	{
		{ name = "ship", path = "../_Games/asteroids/textures/ship.png", pixel_art = true},
		{ name = "ast_1", path = "../_Games/asteroids/textures/asteroid_1.png", pixel_art = true},
		{ name = "ast_2", path = "../_Games/asteroids/textures/asteroid_2.png", pixel_art = true},
		{ name = "bg", path = "../_Games/asteroids/textures/dark_purple_bg.png", pixel_art = true},
		{ name = "proj_1", path = "../_Games/asteroids/textures/projectile_1.png", pixel_art = true},
		{ name = "lives", path = "../_Games/asteroids/textures/lives_ship.png", pixel_art = true},
		{ name = "game_over", path = "../_Games/asteroids/textures/game_over.png", pixel_art = true},
		{ name = "score", path = "../_Games/asteroids/textures/score.png", pixel_art = true},
		{ name = "numbers", path = "../_Games/asteroids/textures/numbers.png", pixel_art = true},
	},
	music = 
	{
		{ name = "NLU", path = "../_Games/asteroids/sounds/music/NLU.mp3"},
		{ name = "IDLN", path = "../_Games/asteroids/sounds/music/IDLN.mp3"}
	},
	sounds_fx = 
	{
		{ name = "laser", path = "../_Games/asteroids/sounds/soundfx/laser.wav"}
	}
}

function LoadAssets()
	for k, v in pairs(AssetDefs.textures) do
		if not AssetManager.add_texture(v.name, v.path, v.pixel_art) then
			print("Failed to load texture [" .. v.name .. "] at path [".. v.path .. "]")
		else
			print("Loaded Texture [" .. v.name .. "]")
		end
	end

	for k, v in pairs(AssetDefs.music) do
		if not AssetManager.add_music(v.name, v.path) then
			print("Failed to load music [" ..v.name .. "] at path [" ..v.path.. "]")
		else
			print("Loaded Music [" .. v.name .. "]")
		end
	end

	for k, v in pairs(AssetDefs.sounds_fx) do
		if not AssetManager.add_soundfx(v.name, v.path) then
			print("Failed to load soundfx [" ..v.name .. "] at path [" ..v.path.. "]")
		else
			print("Loaded soundfx [" .. v.name .. "]")
		end
	end
end