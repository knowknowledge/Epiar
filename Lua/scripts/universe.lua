
-- Use this script for a solar system
-- This is responsible for

-- Generate Ships
number_of_ships = 20
ship_list = {}

for s =1,number_of_ships do
	io.write("Creating Shipt ",s)
	local cur_ship = Ship:new(
		math.random(1000)-500, -- X
		math.random(1000)-500, -- Y
		"Terran FV-5 Frigate", -- Ship Model
		"chase"                 -- Ship Script
		)
	ship_list[s] =  cur_ship
end

