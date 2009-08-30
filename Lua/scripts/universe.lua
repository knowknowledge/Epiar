
-- Use this script for a solar system
-- This is responsible for

-- Generate Ships
number_of_ships = 20
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

	io.write("Center at ",avg_x,",",avg_y,"\n" )
	io.write("Angle ",avg_angle,"\n" )
	io.write("Speed ",avg_speed,"\n" )
	io.write("Vector ",avg_vector,"\n" )
	io.write("\n" )

	-- Move towards the center
	for s =1,number_of_ships do
		cur_ship = ship_list[s]
		EpiarLua.Ship.Rotate(cur_ship, math.random(3) -1 )
		EpiarLua.Ship.Accelerate(cur_ship )
	end
end
	
