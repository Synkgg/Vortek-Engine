AsteroidDefs = 
{
	asteroid_big = 
	{
		group = "asteroids",
		components = 
		{
			transform = {
				position = {x = -100, y = -100},
				scale = {x = 1, y = 1},
				rotation = 0,
			},
			sprite = {
				asset_name = "ast_1",
				width = 120,
				height = 98,
				start_x = 0, start_y = 0,
				layer = 2
			},
			circle_collider = {
				radius = 44
			}
		},
		type = "big",
		min_speed = -0.02,
		max_speed = -0.05,
	},

	asteroid_small = 
	{
		group = "asteroids",
		components = 
		{
			transform = {
				position = {x = -100, y = -100},
				scale = {x = 1, y = 1},
				rotation = 0,
			},
			sprite = {
				asset_name = "ast_2",
				width = 29,
				height = 26,
				start_x = 0, start_y = 0,
				layer = 2
			},
			circle_collider = {
				radius = 13
			}
		},
		type = "small",
		min_speed = -0.01,
		max_speed = -0.05,
	},
}

ShipDefs =
{
	blue_ship = 
	{
		tag = "ship",
		components = 
		{
			transform = {
				position = {x = 320, y = 240},
				scale = {x = 1, y = 1},
				rotation = 0,
			},
			sprite = {
				asset_name = "ship",
				width = 112,
				height = 75,
				start_x = 0, start_y = 0,
				layer = 2
			},
			circle_collider = {
				radius = 56
			}
		}
	}
}

ProjectileDefs =
{
	proj_1 = 
	{
		group = "projectiles",
		components = 
		{
			transform = {
				position = {x = 0, y = 0},
				scale = {x = 1, y = 1},
				rotation = 0,
			},
			sprite = {
				asset_name = "proj_1",
				width = 13,
				height = 37,
				start_x = 0, start_y = 0,
				layer = 2
			},
			circle_collider = {
				radius = 33
			}
		},
		life_time = 2000,
		proj_speed = 1
	}
}

HudDefs = 
{
	lives = 
	{
		group = "lives",
		components = 
		{
			transform = {
				position = {x = 0, y = 0},
				scale = {x = 1, y = 1},
				rotation = 0,
			},
			sprite = {
				asset_name = "lives",
				width = 37,
				height = 26,
				start_x = 0, start_y = 0,
				layer = 10
			},
		}
	},
	score = 
	{
		tag = "score",
		components = 
		{
			transform = {
				position = {x = 200, y = 0},
				scale = {x = 1, y = 1},
				rotation = 0,
			},
			sprite = {
				asset_name = "score",
				width = 200.0,
				height = 50.0,
				start_x = 0, start_y = 0,
				layer = 10
			},
		}
	},
	number = 
	{
		group = "numbers",
		components = 
		{
			transform = {
				position = {x = 380, y = 10},
				scale = {x = 1, y = 1},
				rotation = 0,
			},
			sprite = {
				asset_name = "numbers",
				width = 20,
				height = 30,
				start_x = 0, start_y = 0,
				layer = 10
			},
		}
	},
	game_over = 
	{
		tag = "game_over",
		components = 
		{
			transform = {
				position = {x = (WINDOW_WIDTH / 2) - 300, y = (WINDOW_HEIGHT / 2) - 75},
				scale = {x = 3, y = 3},
				rotation = 0,
			},
			sprite = {
				asset_name = "game_over",
				width = 200.0,
				height = 50.0,
				start_x = 0, start_y = 0,
				layer = 10,
				bHidden = true,
			},
		}
	},
}