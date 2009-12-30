-- Use this script for a solar system


-- Keyboard States:
KEYUP, KEYDOWN, KEYPRESSED, KEYTYPED = 0,1,2,3
SDLK_BACKSPACE, SDLK_ESCAPE = 9, 27
SDLK_RSHIFT, SDLK_LSHIFT = 47, 48
SDLK_UP, SDLK_DOWN, SDLK_RIGHT, SDLK_LEFT = 17, 18, 19, 20

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
	if #PreSteps >0 then
		for i,pre_func in ipairs(PreSteps) do
			pre_func()
		end
	end
	MoveShips()
	if #PostSteps >0 then
		for i,post_func in ipairs(PostSteps) do
			post_func()
		end
	end
end

--------------------------------------------------------------------------------
-- Basic Utilities

function togglePause()
	io.write("Toggling...\n")
	if 1 == Epiar.ispaused() then
		io.write("Un Pause\n")
		Epiar.unpause()
	else
		io.write("Pause\n")
		Epiar.pause()
	end
end

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
	a = {}
	a[1] = "Missile"
	a[2] = "Slow Missile"
	a[3] = "Strong Laser"
	a[4] = "Minigun"
	
	-- Generate Ships
	for s =1,number_of_ships do
		cur_ship = Ship:new(
				math.random(1000)-500+X, -- X
				math.random(1000)-500+Y, -- Y
				shiptypes[math.random(#shiptypes)],
				"chase"                 -- Ship Script
				)
		Ship.SetRadarColor(cur_ship,0,255,0)
		AIPlans[ Ship.GetID(cur_ship) ] = newPlan()
		
		--Randomly assign a weapon to everyone
		i = math.random(4)
		Ship.AddWeapon( cur_ship, a[i] )
		Ship.AddAmmo(cur_ship, a[i],100 )
	end
end

-- Execute the current plan of each AI
function MoveShips()
	ships = Epiar.ships()
	-- Move Non-Player ships
	for s =1, #ships do
		cur_ship = ships[s]
		n = Ship.GetID(cur_ship)
		AIPlans[n].plan( cur_ship, AIPlans[n].time )
		AIPlans[n].time = AIPlans[n].time -1
		-- When the current plan is complete, pick a new plan
		if AIPlans[n].time == 0 then
			AIPlans[n] = newPlan()
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
	Ship.Fire(cur_ship )
	Ship.Accelerate(cur_ship )
end
registerInit(planetTraffic)
registerPlan(aimCenter)

function buy(model)
	io.write("Player just bought "..model.."\n")
	Ship.SetModel(Epiar.player(),model)
	return 1
end


function store()
	Epiar.pause()
	if storefront ~=nil then return end

	-- The Store layout parameters
	width = 820
	height = 500
	pad = 10
	box = 120
	button_h = 30
	button_w = 100
	row,col = 1,1	
	getPos = function(c,r)
		pos_x = pad*(col)+box*(col-1)
		pos_y = pad*(row)+box*(row-1)
		return pos_x,pos_y
	end

	-- Layout the Store in a grid
	storefront = UI:newWindow( 30,30,width,height,"Ship Yard")
	models = Epiar.models()
	for m =1,#models do
		pos_x,pos_y = getPos(col,row)
		-- When there isn't enough room, wrap to the next row.
		if  pos_x+box >= width then 
			col=1; row=row+1
			pos_x,pos_y = getPos(col,row)
		end

		UI.add(storefront, UI:newButton(
			pos_x+(box-button_w)/2,
			pos_y,
			button_w,button_h, models[m],
			" Epiar.unpause(); buy(\""..models[m].."\"); UI:close(storefront);storefront=nil "))
		UI.add(storefront, UI:newPicture(
			pos_x,
			pos_y + button_h,
			box,box-button_h,models[m]))

		col =col+1
	end
end

function ui_demo()
	if demo_win ~= nil then return end

	-- Create the widgets
	demo_win = UI:newWindow( 200, 100, 400, 300, "User Interface Demo")
	demo_text1 = UI:newTextbox( 50, 50, 100, 1)
	demo_text2 = UI:newTextbox( 250, 50, 100, 1)
	demo_check = UI:newCheckbox(50, 100, 0, "Toggle This")

	-- Modify the Widgets
	demo_trigger = function ()
		if 1==UI.IsChecked(demo_check) then
			io.write('\nchecked!\n')
		else
			io.write('\nnot checked!\n')
		end
		UI:close(demo_win)
		demo_win = nil;
	end
	demo_swap = function()
		s1 = UI.GetText(demo_text1)
		s2 = UI.GetText(demo_text2)
		UI.setText(demo_text1,s2)
		UI.setText(demo_text2,s1)
	end

	-- Attach the widgets to the window
	UI.add(demo_win, demo_text1)
	UI.add(demo_win, demo_text2)
	UI.add(demo_win, demo_check)
	UI.add(demo_win, UI:newButton( 175, 50, 14*3, 18, "<->", "demo_swap()"))
	UI.add(demo_win, UI:newButton( 152, 262-45, 96, 25, "TOGGLE", "UI.setChecked(demo_check, not UI.IsChecked(demo_check) )"))
	UI.add(demo_win, UI:newButton( 152, 262, 96, 25, "OK", "demo_trigger()"))
end


--registerInit(store)
Epiar.RegisterKey(string.byte('s'),KEYTYPED,"store()")

Epiar.RegisterKey(string.byte('p'),KEYTYPED,"togglePause()")
Epiar.RegisterKey(string.byte('g'),KEYTYPED,"ui_demo()")
-- pause should 1) not be implemented in lua and 2) should respond to keytyped events, not keydown events, else
-- a 'p' typed into the UI will also pause the game. this makes no sense. however, if a UI text input has no
-- focus, the UI will pass the typed event down the chain and pause should reach it eventually

-- Register the player functions
Epiar.RegisterKey(SDLK_UP, KEYPRESSED, "Ship.Accelerate(Epiar.player())" )
Epiar.RegisterKey(SDLK_LEFT, KEYPRESSED, "Ship.Rotate(Epiar.player(),30)" )
Epiar.RegisterKey(SDLK_RIGHT, KEYPRESSED, "Ship.Rotate(Epiar.player(),-30)" )
Epiar.RegisterKey(SDLK_DOWN, KEYPRESSED, "Ship.Rotate(Epiar.player(),Ship.directionTowards(Ship.GetMomentumAngle(Epiar.player()) + 180 ))" )
Epiar.RegisterKey(string.byte('c'), KEYPRESSED, "Ship.Rotate(Epiar.player(),Ship.directionTowards(Epiar.player(), 0,0))" )
Epiar.RegisterKey(SDLK_RSHIFT, KEYPRESSED, "Ship.ChangeWeapon(Epiar.player())" )
Epiar.RegisterKey(SDLK_LSHIFT, KEYPRESSED, "Ship.ChangeWeapon(Epiar.player())" )
Epiar.RegisterKey(string.byte(' '), KEYPRESSED, "Ship.Fire(Epiar.player())" )

Ship.AddWeapon( Epiar.player(), "Minigun" )
Ship.AddWeapon( Epiar.player(), "Missile" )
Ship.AddAmmo( Epiar.player(), "Missile",100 )


--------------------------------------------------------------------------------
-- Load Scenarios

dofile "Resources/Scripts/basics.lua"
--dofile "Resources/Scripts/tag.lua"
dofile "Resources/Scripts/swarm.lua"
