
-- Use this script for a solar system

-- Lua Global variables
AIPlans = {}

function CreateShips(number_of_ships, X, Y)
	shiptypes = Epiar.models()
	-- Generate Ships
	for s =1,number_of_ships do
		cur_ship = EpiarLua.Ship:new(
				math.random(1000)-500+X, -- X
				math.random(1000)-500+Y, -- Y
				shiptypes[math.random(#shiptypes)],
				"chase"                 -- Ship Script
				)
		EpiarLua.Ship.SetRadarColor(cur_ship,0,255,0)
		table.insert(AIPlans, newPlan() )
	end
end

-- The swarm is the net average of all non-player ships
swarm = {}
swarm.reset = function()
	swarm.avg_x= 0
	swarm.avg_y= 0
	swarm.avg_angle = 0
	swarm.avg_vector= 0
	swarm.avg_speed = 0
end
swarm.findAverage = function()
	local avg_x= 0
	local avg_y= 0
	local avg_angle = 0
	local avg_vector= 0
	local avg_speed = 0
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	-- Average the statistics of each ship
	for s =1,#ships do
		cur_ship = ships[s]
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
	swarm.avg_x = avg_x / #ships
	swarm.avg_y = avg_y / #ships
	swarm.avg_angle = avg_angle / #ships
	swarm.avg_vector = avg_vector / #ships
	swarm.avg_speed = avg_speed / #ships
end
swarm.reset()

function distfrom( pt1_x,pt1_y, pt2_x,pt2_y)
	x_diff = (pt1_x - pt2_x)
	y_diff = pt1_y - pt2_y
	return math.sqrt(x_diff*x_diff + y_diff*y_diff)
end

it = {}
it.ship =0
it.countdown=100
it.pic = EpiarLua.UI:newPicture(60,30,100,30, EpiarLua.Ship.GetModelName(Epiar.player()) )
it.label = EpiarLua.UI:newLabel(90,100,"You're It")
it.findClosest = function()
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	it.target=-1
	it.target_dist= 100000
	it.target_x,target_y= 10000,10000
	it.x,it.y = EpiarLua.Ship.GetPosition(ships[it.ship])

	-- Find the closest ship to whomever is IT
	for other=1, #ships do 
		if other ~= it.ship then
			other_x,other_y = EpiarLua.Ship.GetPosition(ships[other])
			dist = distfrom(other_x,other_y,it.x,it.y)
			if dist < it.target_dist then
				it.target = other
				it.target_dist= dist
				it.target_x,it.target_y = other_x,other_y
			end
		end
	end
end
it.tag = function(target)
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	if target >= #ships then
		return 1
	end
	-- the old IT now runs
	AIPlans[it.ship] = {}
	AIPlans[it.ship].time=20
	AIPlans[it.ship].plan=aimAwayFromIT
	EpiarLua.Ship.SetRadarColor(ships[it.ship],0,255,0)
	
	--shake the camera
	other_x,other_y = EpiarLua.Ship.GetPosition(ships[target])
	if it.ship == 0 then
		Epiar.shakeCamera(100, 3, other_x,other_y)
	elseif target == 0 then
		Epiar.shakeCamera(100, 3, it.x,it.y)
	end

	-- The new it doesn't become active for 100 ticks
	it.ship=target
	it.countdown = 100
	AIPlans[it.ship] = {}
	AIPlans[it.ship].time=0
	AIPlans[it.ship].plan=chaseClosest
	EpiarLua.Ship.SetRadarColor(ships[it.ship],255,0,0)
end

--
-- AI Plans
--

function chaseClosest(cur_ship,timeleft)
	-- in the game of tag, this is 'it'
	EpiarLua.Ship.Rotate(cur_ship, 
		EpiarLua.Ship.directionTowards(cur_ship, it.target_x, it.target_y) )
	EpiarLua.Ship.Accelerate(cur_ship )
end

function aimCenter(cur_ship,timeleft)
	-- direction towards the center or the universe
	if timeleft%3 ==0 then
		EpiarLua.Ship.Rotate(cur_ship,
			EpiarLua.Ship.directionTowards(cur_ship, 0,0) )
	end
	EpiarLua.Ship.Accelerate(cur_ship )
end

function aimTowardsIT(cur_ship,timeleft)
	-- direction towards the ship that is IT
	EpiarLua.Ship.Rotate(cur_ship,
		EpiarLua.Ship.directionTowards(cur_ship, it.x, it.y) )
	if timeleft %2 == 0 then
		EpiarLua.Ship.Accelerate(cur_ship )
	end
end

function aimAwayFromIT(cur_ship,timeleft)
	-- direction away from the ship that is IT
	EpiarLua.Ship.Rotate(cur_ship,
		-EpiarLua.Ship.directionTowards(cur_ship, it.x, it.y) )
	EpiarLua.Ship.Accelerate(cur_ship )
end

function aimSwarmCenter(cur_ship,timeleft)
	-- direction towards the center of the swarm
	if timeleft %2 == 0 then
		EpiarLua.Ship.Rotate(cur_ship, 
			EpiarLua.Ship.directionTowards(cur_ship, swarm.avg_x, swarm.avg_y) )
	end
	EpiarLua.Ship.Accelerate(cur_ship )
end

function aimSwarmDirection(cur_ship,timeleft)
	-- direction away from the ship that is IT
	EpiarLua.Ship.Rotate(cur_ship,
		EpiarLua.Ship.directionTowards(cur_ship, swarm.avg_angle) )
	EpiarLua.Ship.Accelerate(cur_ship )
end

function zigzag(cur_ship,timeleft)
	-- direction away from the ship that is IT
	if timeleft % 10 <=3 then
		EpiarLua.Ship.Rotate(cur_ship, 1)
	elseif timeleft % 10 >=7 then
		EpiarLua.Ship.Rotate(cur_ship, -1)
	end
	EpiarLua.Ship.Accelerate(cur_ship )
end

-- Generate a new plan from the list above
function newPlan()
	randomChoice = math.random(100)
	theNewPlan = {}
	theNewPlan.time = math.random(30)
	if randomChoice <30 then
		theNewPlan.plan=aimAwayFromIT
	elseif randomChoice<40 then
		theNewPlan.plan=zigzag
	elseif randomChoice<75 then
		theNewPlan.plan=aimSwarmCenter
	elseif randomChoice<95 then
		theNewPlan.plan=aimSwarmDirection
	else
		theNewPlan.plan=aimTowardsIT
	end
	return theNewPlan
end


-- The C++ engine calls this function once per tick.
-- This function is responsible for directing all of the game AI.
function Update ()
	-- Calculate variables
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	swarm.findAverage()
	it.findClosest()
	-- Move Non-Player ships
	for s =1, #ships do
		cur_ship = ships[s]
		--zigzag(cur_ship,tick)
		AIPlans[s].plan( cur_ship, AIPlans[s].time )
		AIPlans[s].time = AIPlans[s].time -1
		-- When the current plan is complete, pick a new plan
		if AIPlans[s].time == 0 then 
			x,y = EpiarLua.Ship.GetPosition(cur_ship)
			if distfrom(x,y,0,0) >1000 then
				AIPlans[s] = {plan=aimCenter,time=300}
			else
				AIPlans[s] = newPlan()
			end
		end
	end

	-- Set the Who's It? Dashboard to the correct Image
	EpiarLua.UI.setPicture(it.pic, EpiarLua.Ship.GetModelName(ships[it.ship]) )
	EpiarLua.UI.rotatePicture(it.pic, EpiarLua.Ship.GetAngle(ships[it.ship]) )

	if it.countdown==0 then
		-- Show who's it
		if it.ship==0 then
			EpiarLua.UI.setText(it.label,"You're IT!")
		else
			EpiarLua.UI.setText(it.label,"Player "..(it.ship).." is IT!")
		end
		-- Is someone else it now?
		if it.target_dist < 200 then 
			it.tag(it.target)
		end
	else
		it.countdown= (it.countdown)-1
		-- Update the countdown only every 10th tick
		if it.countdown%10==0 then
			if it.ship==0 then
				EpiarLua.UI.setText(it.label,"You're IT in: "..(it.countdown/10))
			else
				EpiarLua.UI.setText(it.label,"Player "..(it.ship).." is IT in: "..(it.countdown/10))
			end
		end
	end
end

-- Functions to use from the console. ( Enter the console by hitting backtick. )

function close() -- Close all the windows
	EpiarLua.UI:close()
end

function pauseMessage(message)
	if 1 == Epiar.ispaused() then return end
	pauseWin= EpiarLua.UI:newWindow( 400,100,320,150,"Paused",
		EpiarLua.UI:newLabel(160,70,message),
		EpiarLua.UI:newButton(110,100,100,30,"Unpause","Epiar.unpause();EpiarLua.UI:close(pauseWin)")
		)
	Epiar.pause()
end

-- Create windows
menuWin = EpiarLua.UI:newWindow( 900,200,120,250,"Menu",
	EpiarLua.UI:newButton(10,40,100,30,"Pause","pauseMessage('You hit the pause button')"),
	EpiarLua.UI:newButton(10,140,100,30,"IT","it.tag(0)"),
	EpiarLua.UI:newButton(10,190,100,30,"NOT IT","it.tag(math.random(#(Epiar.ships())))")
	)
tagWin = EpiarLua.UI:newWindow( 840,500,180,130,"Who's IT?",
	it.pic,
	it.label
	)
	
-- Create Some ships around the planets
planets = Epiar.planets()
for p=1,#planets do
	traffic = Epiar.Planet.Traffic(planets[p])
	x,y = Epiar.Planet.Position(planets[p])
	CreateShips(traffic,x,y)
end

