
-- Use this script for a solar system

-- Lua Global variables
shipList = {}  -- The ships that Lua has access to.  TODO: This list should probably be generated at each tick from a c++ call.
frozen=0 -- Setting this to 1 causes all AI to stop 'thinking' and just drift.
ticks=0

function CreateShips(number_of_ships, X, Y)
	-- Generate Ships
	for s =1,number_of_ships do
		io.write("Creating Ship ",s)
		cur_ship = EpiarLua.Ship:new(
				math.random(1000)-500+X, -- X
				math.random(1000)-500+Y, -- Y
				"Terran FV-5 Frigate", -- Ship Model
				"chase"                 -- Ship Script
				)
		-- io.write( cur_ship["__self"] )  -- userdata
		-- io.write( cur_ship["__index"] ) -- table
		--print( cur_ship.__index )
		table.insert(shipList, cur_ship )
		x,y = EpiarLua.Ship.GetPosition(cur_ship)
		--EpiarLua.Ship.Rotate(cur_ship,1)
	end

	io.write("Ships Total: ", #shipList ,"\n")
end

function SwarmAverage()
	-- Find the swarm center
	avg_x,avg_y = 0,0
	avg_angle = 0
	avg_vector= 0
	avg_speed = 0
	for s =1,#shipList do
		cur_ship = shipList[s]
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
	avg_x = avg_x / #shipList
	avg_y = avg_y / #shipList
	avg_angle = avg_angle / #shipList
	avg_vector = avg_vector / #shipList
	avg_speed = avg_speed / #shipList
    return avg_x,avg_y,avg_angle,avg_vector,avg_speed
end

-- The C++ engine calls this function once per tick.
-- This function is responsible for directing all of the game AI.
function Update ()
	if frozen>0 then
		return 1
	end

	avg_x,avg_y,avg_angle,avg_vector,avg_speed = SwarmAverage()

	-- Move towards the center
	for s =1, # shipList do
		cur_ship = shipList[s]
		x,y = EpiarLua.Ship.GetPosition(cur_ship)
		dir_point = EpiarLua.Ship.directionTowards(cur_ship, avg_x, avg_y) -- direction towards the center of the swarm
		dir_aim = EpiarLua.Ship.directionTowards(cur_ship, avg_angle ) -- direction towards the swarm direction
		dir_center = EpiarLua.Ship.directionTowards(cur_ship, 0,0) -- direction towards the center of the universe

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

-- Functions to use from the console. ( Enter the console by hitting backtick. )
-- TODO: Remove these functions, they are only interesting for debugging and even very useful then.
function stop()
	frozen=1
end

function go()
	frozen=0
end

function open(x,y,w,h,someString) -- Create an Arbitrary Window
	EpiarLua.UI:newWindow(x,y,w,h,someString)
end

function close() -- Close all the windows
	EpiarLua.UI:close()
end

function menu() -- Generate a test window
	-- Create windows
	menuWin = EpiarLua.UI:newWindow( 900,200,120,250,"Menu",
		EpiarLua.UI:newButton(10,40,100,30,"Pause","Epiar.pause()"),
		EpiarLua.UI:newButton(10,90,100,30,"Unpause","Epiar.unpause()"),
		EpiarLua.UI:newButton(10,140,100,30,"AI Drift","stop()"),
		EpiarLua.UI:newButton(10,190,100,30,"AI Think","go()")
		)
end
	
-- Create Some ships around the planets
-- TODO, Lua should create these ships based off of information found in the planets-default.xml
if 1 >0 then
	CreateShips(3,345,215)
	CreateShips(6,-40,-135)
	CreateShips(6,4640,-735)
end
