-- Use this script for a solar system

--------------------------------------------------------------------------------
-- Init is a list of functions to be run when the game (re)starts

Init = {}
function registerInit(step)
    table.insert(Init,step)
end

--------------------------------------------------------------------------------
-- Registered Plans for AI to choose from

Plans = {}
function registerPlan(plan)
    table.insert(Plans,plan)
end
AIPlans = {}

--------------------------------------------------------------------------------
-- Steps taken during each Update

PreSteps = {}
PostSteps = {}
function registerPreStep(step)
    table.insert(PreSteps,step)
end
function registerPostStep(step)
    table.insert(PostSteps,step)
end

function Start()
	io.write(string.format("\tInit: %d\n\tPlans: %d\n\tPreSteps: %d\n\tPostSteps: %d\n", #Init, #Plans, #PreSteps, #PostSteps ))
    for i,func in ipairs(Init) do
        func()
    end
end

function Update()
    for i,pre_func in ipairs(PreSteps) do
        pre_func()
    end
    MoveShips()
    for i,post_func in ipairs(PostSteps) do
        post_func()
    end
end

--------------------------------------------------------------------------------
-- Basic Utilities

-- Pause the Game with a given message
function pauseMessage(message)
	if 1 == Epiar.ispaused() then return end
	pauseWin= UI:newWindow( 400,100,320,150,"Paused",
		UI:newLabel(160,40,message,1),
		UI:newButton(110,80,100,30,"Unpause","Epiar.unpause();UI:close(pauseWin)")
		)
	Epiar.pause()
end
--registerInit(function() pauseMessage("hello world") end)

-- Calculate the Distance between two points
function distfrom( pt1_x,pt1_y, pt2_x,pt2_y)
	x_diff = (pt1_x - pt2_x)
	y_diff = pt1_y - pt2_y
	return math.sqrt(x_diff*x_diff + y_diff*y_diff)
end

-- Generate a new plan from the list above
function newPlan()
	theNewPlan = {}
	planNum = math.random(#Plans)
	theNewPlan.time = math.random(30)
    theNewPlan.plan = Plans[ planNum ]
	return theNewPlan
end

-- Create some Random Ships around a Planet
function CreateShips(number_of_ships, X, Y)
	shiptypes = Epiar.models()
	-- Generate Ships
	for s =1,number_of_ships do
		cur_ship = Ship:new(
				math.random(1000)-500+X, -- X
				math.random(1000)-500+Y, -- Y
				shiptypes[math.random(#shiptypes)],
				"chase"                 -- Ship Script
				)
		Ship.SetRadarColor(cur_ship,0,255,0)
		table.insert(AIPlans, newPlan() )
	end
end

-- Execute the current plan of each AI
function MoveShips()
	ships = Epiar.ships()
	-- Move Non-Player ships
	for s =1, #ships do
		cur_ship = ships[s]
		AIPlans[s].plan( cur_ship, AIPlans[s].time )
		AIPlans[s].time = AIPlans[s].time -1
		-- When the current plan is complete, pick a new plan
		if AIPlans[s].time == 0 then 
			AIPlans[s] = newPlan()
		end
	end
end

--------------------------------------------------------------------------------
-- Necessary functions for now.

-- Create Some ships around the planets
function planetTraffic()
    planets = Epiar.planets()
    for p=1,#planets do
        traffic = Planet.Traffic(planets[p])
        x,y = Planet.Position(planets[p])
        CreateShips(traffic,x,y)
    end
end

function aimCenter(cur_ship,timeleft)
	-- direction towards the center or the universe
	if timeleft%3 ==0 then
		Ship.Rotate(cur_ship,
			Ship.directionTowards(cur_ship, 0,0) )
	end
	Ship.Accelerate(cur_ship )
end
registerInit(planetTraffic)
registerPlan(aimCenter)

function hitEveryOne()
	pos_x, pos_y = Ship.GetPosition(Epiar.player())
	ships = Epiar.ships( pos_x, pos_y, 100 )
	for s =1, #ships do
		ship = ships[s]
		Ship.Damage(ship,1)
		pct = Ship.GetHull(ship)
		io.write(string.format("Ship %d is at %f%% Hull\n",Ship.GetID(ships[s]),pct))
		if pct <= 0 then
			Ship.Explode(ships[s])
		else
			Ship.SetRadarColor(ships[s],255 *pct,0 ,0)
		end
	end
end
registerPostStep(hitEveryOne)

function buy(model)
	io.write("Player just bought "..model.."\n")
	Ship.SetModel(Epiar.player(),model)
	return 1
end

function store()
	Epiar.pause()
	width = 820
	height = 500
	pad = 10
	box = 130
	storefront = UI:newWindow( 30,30,width,height,"Ship Yard")
	models = Epiar.models()
	row,col = 1,1	
	for m =1,#models do
		UI.add(storefront, UI:newButton(pad*(col)+box*(col-1), pad*(row)+box*(row-1), 100,30, models[m], " Epiar.unpause(); buy(\""..models[m].."\"); UI:close(storefront); ")) --
		UI.add(storefront, UI:newPicture(pad*(col+2)+box*(col-1),30+(pad)*(row+2)+box*(row-1),box,box,models[m]))
		col =col+1
		-- When there isn't enough room, wrap to the next row.
		if pad*(col+2)+box*(col-1) >= width then 
			col=1; row=row+1
		end
	end
end

--------------------------------------------------------------------------------
-- Load Scenarios

dofile "Resources/Scripts/basics.lua"
--dofile "Resources/Scripts/tag.lua"
--dofile "Resources/Scripts/swarm.lua"
