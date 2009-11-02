
-- Use this script for a solar system

-- Lua Global variables
shipList = {}  -- The ships that Lua has access to.  TODO: This list should probably be generated at each tick from a c++ call.
frozen=0 -- Setting this to 1 causes all AI to stop 'thinking' and just drift.
ticks=0
player  = Epiar.player()
shipList[0] = player
it_countdown=100

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
		table.insert(shipList, cur_ship )
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

function distfrom( pt1_x,pt1_y, pt2_x,pt2_y)
	x_diff = (pt1_x - pt2_x)
	y_diff = pt1_y - pt2_y
	return math.sqrt(x_diff*x_diff + y_diff*y_diff)
end

function closestToIT()
	target=-1
	mindist = 100000
	min_x,min_y = 10000,10000
	it_x,it_y = EpiarLua.Ship.GetPosition(shipList[it])

	-- Find the closest ship to whomever is IT
	for other=0, #shipList do 
		if not(other == it) then
			other_x,other_y = EpiarLua.Ship.GetPosition(shipList[other])
			dist = distfrom(other_x,other_y,it_x,it_y)
			if dist < mindist then
				target = other
				mindist = dist
				min_x,min_y = other_x,other_y
			end
		end
	end
	return target, min_x, min_y, mindist
end


-- The C++ engine calls this function once per tick.
-- This function is responsible for directing all of the game AI.
function Update ()
	if frozen>0 then
		return 1
	end


	avg_x,avg_y,avg_angle,avg_vector,avg_speed = SwarmAverage()

	player_x,player_y = EpiarLua.Ship.GetPosition(player)
	it_x,it_y = EpiarLua.Ship.GetPosition(shipList[it])

	-- Move
	for s =1, # shipList do
		cur_ship = shipList[s]
		x,y = EpiarLua.Ship.GetPosition(cur_ship)

		if s == it then -- IT
			dir_closest = EpiarLua.Ship.directionTowards(cur_ship, min_x,min_y) -- direction towards the closest target
			EpiarLua.Ship.Rotate(cur_ship, dir_closest)
		else -- Not IT
			if math.sqrt(x*x + y*y) >3000 then 
				dir_center = EpiarLua.Ship.directionTowards(cur_ship, 0,0) -- direction towards the center of the universe
				EpiarLua.Ship.Rotate(cur_ship, dir_center)
			else
				if math.random(100) <10 then
					dir_it = EpiarLua.Ship.directionTowards(cur_ship, it_x, it_y) -- direction towards the ship that is IT
					EpiarLua.Ship.Rotate(cur_ship, dir_it)
				elseif math.random(100) <40 then
					dir_it = EpiarLua.Ship.directionTowards(cur_ship, it_x, it_y) -- direction away from the ship that is IT
					EpiarLua.Ship.Rotate(cur_ship, -dir_it)
				elseif math.random(100) <70 then
					dir_swarm_center = EpiarLua.Ship.directionTowards(cur_ship, avg_x, avg_y) -- direction towards the center of the swarm
					EpiarLua.Ship.Rotate(cur_ship, dir_swarm_center)
				else
					dir_swarm_aim = EpiarLua.Ship.directionTowards(cur_ship, avg_angle ) -- direction towards the swarm direction
					EpiarLua.Ship.Rotate(cur_ship, dir_swarm_aim)
				end
			end

		end
		EpiarLua.Ship.Accelerate(cur_ship )
	end

	if it_countdown==0 then
		-- Is someone else it now?
		target, min_x, min_y, mindist = closestToIT()
		if mindist < 200 then 
			setIt(target)
		end
	else
		it_countdown= it_countdown-1
		if it_countdown==0 then
			Epiar.echo("Let the games begin")
		elseif it_countdown%10==0 then
			Epiar.echo("Player "..it.." is counting down: "..it_countdown/10)
		end
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

-- Tag Functions
function setIt(target)
	it=target
	it_countdown = 100
	if it==0 then
	Epiar.echo("TAG!  You're now it!")
	--pauseMessage("TAG!  You're now it!")
	else
	Epiar.echo("Ship "..target.." is now IT.")
	--pauseMessage("Ship "..target.." is now IT.")
	end
end
function whosit()
	if it==0 then
		Epiar.echo("You are it")
	else
		Epiar.echo("Player "..it.." is it.")
	end
end
function whosClosest()
	it_x,it_y = EpiarLua.Ship.GetPosition(shipList[it])
	target, min_x, min_y, mindist = closestToIT()
	Epiar.echo("Closest ship to IT "..it.." at ("..it_x..","..it_y..") is ".. target.." at ("..min_x..","..min_y..") "..mindist)
end

function pauseMessage(message)
	Epiar.pause()
	menuWin = EpiarLua.UI:newWindow( 900,200,120,250,"Paused",
		EpiarLua.UI:newLabel(10,40,message),
		EpiarLua.UI:newButton(10,90,100,30,"Unpause","Epiar.unpause()")
		)
end

function menu() -- Generate a test window
	-- Create windows
	menuWin = EpiarLua.UI:newWindow( 900,200,120,450,"Menu",
		EpiarLua.UI:newButton(10,40,100,30,"Pause","Epiar.pause()"),
		EpiarLua.UI:newButton(10,90,100,30,"Unpause","Epiar.unpause()"),
		EpiarLua.UI:newButton(10,140,100,30,"AI Drift","stop()"),
		EpiarLua.UI:newButton(10,190,100,30,"AI Think","go()"),
		EpiarLua.UI:newButton(10,240,100,30,"IT","setIt(0)"),
		EpiarLua.UI:newButton(10,290,100,30,"NOT IT","setIt(math.random(#shipList))")
		)
end
	
-- Create Some ships around the planets
-- TODO, Lua should create these ships based off of information found in the planets-default.xml
if 1 >0 then
	CreateShips(3,345,215)
	CreateShips(6,-40,-135)
	CreateShips(6,4640,-735)
end
it=math.random(#shipList)
it=0


