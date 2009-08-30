
-- Use this script for a solar system
-- This is responsible for

-- Generate Ships
number_of_ships = 200
ship_list = {}

for s =1,number_of_ships do
	io.write("Creating Ship ",s)
	cur_ship = EpiarLua.Ship:new(
		math.random(1000)-500, -- X
		math.random(1000)-500, -- Y
		"Terran FV-5 Frigate", -- Ship Model
		"chase"                 -- Ship Script
		)
	-- io.write( cur_ship["__self"] )  -- userdata
	-- io.write( cur_ship["__index"] ) -- table
	--print( cur_ship.__index )
	ship_list[s] =  cur_ship
	x,y = EpiarLua.Ship.GetPosition(cur_ship)
	--EpiarLua.Ship.Rotate(cur_ship,1)
end

io.write("Ships Total: ", #ship_list ,"\n")

function Update ()
	-- Find the swarm center
	avg_x,avg_y = 0,0
	avg_angle = 0
	avg_vector= 0
	avg_speed = 0
	for s =1,#ship_list do
		cur_ship = ship_list[s]
		x,y = EpiarLua.Ship.GetPosition(cur_ship)
		avg_x = avg_x + x
		avg_y = avg_y + y
		angle = EpiarLua.Ship.GetAngle(cur_ship)
		vec   = EpiarLua.Ship.GetMomentumAngle(cur_ship)
		speed = EpiarLua.Ship.GetMomentumSpeed(cur_ship)
		avg_angle = avg_angle + angle
		avg_vector= avg_vector+ vec
		avg_speed = avg_speed + speed
	end
	avg_x = avg_x / #ship_list
	avg_y = avg_y / #ship_list
	avg_angle = avg_angle / #ship_list
	avg_vector = avg_vector / #ship_list
	avg_speed = avg_speed / #ship_list

	-- Move towards the center
	for s =1,number_of_ships do
		cur_ship = ship_list[s]
		x,y = EpiarLua.Ship.GetPosition(cur_ship)
		dir_point = EpiarLua.Ship.directionTowards(cur_ship, avg_x, avg_y)
		dir_aim = EpiarLua.Ship.directionTowards(cur_ship, avg_angle )
		dir_center = EpiarLua.Ship.directionTowards(cur_ship, 0,0)

		if math.sqrt(x*x + y*y) >1000 then 
			EpiarLua.Ship.Rotate(cur_ship, dir_center)
		else
			if dir_point == dir_aim then
				EpiarLua.Ship.Rotate(cur_ship, dir_aim)
			else
				if math.random(2) == 1 then
					EpiarLua.Ship.Rotate(cur_ship, dir_point)
				else
					EpiarLua.Ship.Rotate(cur_ship, dir_aim)
				end
			end
		end
		EpiarLua.Ship.Accelerate(cur_ship )
	end
end
	
