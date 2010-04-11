-- Use this script for a solar system


--------------------------------------------------------------------------------
-- Init is a list of functions to be run when the game (re)starts

Init = {}
--- Initialization functions
function registerInit(step)
	table.insert(Init,step)
end

--------------------------------------------------------------------------------
-- Registered Plans for AI to choose from

Plans = {}
--- List of plans for AI
function registerPlan(plan)
	table.insert(Plans,plan)
end
AIPlans = {}

--------------------------------------------------------------------------------
-- Steps taken during each Update

PreSteps = {}
PostSteps = {}
--- Steps before each update
function registerPreStep(step)
	table.insert(PreSteps,step)
end
--- Steps after each update
function registerPostStep(step)
	table.insert(PostSteps,step)
end

--- Run the functions
function Start()
	io.write(string.format("\tInit: %d\n\tPlans: %d\n\tPreSteps: %d\n\tPostSteps: %d\n", #Init, #Plans, #PreSteps, #PostSteps ))
	for i,func in ipairs(Init) do
		func()
	end
end

--- Update function
function Update()
	if #PreSteps >0 then
		for i,pre_func in ipairs(PreSteps) do
			pre_func()
		end
	end
	if #PostSteps >0 then
		for i,post_func in ipairs(PostSteps) do
			post_func()
		end
	end
end

--------------------------------------------------------------------------------
-- Basic Utilities

--- Pause the game
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

--- Pause the Game with a given message
function pauseMessage(message)
	if 1 == Epiar.ispaused() then return end
	pauseWin= UI.newWindow( 400,100,320,150,"Paused",
		UI.newLabel(160,40,message,1),
		UI.newButton(110,80,100,30,"Unpause","Epiar.unpause();pauseWin:close()")
		)
	Epiar.pause()
end

--- For debugging
function godmode()
	function heal()
		PLAYER:Repair(10000)
	end
	registerPostStep(heal)
end
--godmode() -- Uncomment this line to never die

--- Calculate the Distance between two points
function distfrom( pt1_x,pt1_y, pt2_x,pt2_y)
	x_diff = (pt1_x - pt2_x)
	y_diff = pt1_y - pt2_y
	return math.sqrt(x_diff*x_diff + y_diff*y_diff)
end

--- Generate a new plan from the list above
function newPlan()
	theNewPlan = {}
	planNum = math.random(#Plans)
	theNewPlan.time = math.random(1000)
	theNewPlan.plan = Plans[ planNum ]
	return theNewPlan
end

--- Creates a new ship
function createShip(X,Y,model,engine)
	plans = {"Hunter","Trader"}
	cur_ship = Ship.new(X,Y,model,engine,plans[math.random(2)])
	cur_ship:SetRadarColor(0,255,0)
	return cur_ship
end

--- Creates a random ship
function createRandomShip(X,Y,Range,models,engines,weapons)
	if models==nil then
		models = Epiar.models()
	end
	if engines==nil then
		engines = Epiar.engines()
	end
	if weapons==nil then
		weapons = Epiar.weapons()
	end
	X = X + math.random(Range)-Range/2
	Y = Y + math.random(Range)-Range/2
	model = models[math.random(#models)]
	engine = engines[math.random(#engines)]
	s = createShip(X,Y,model,engine)
	attachRandomWeapon(s,weapons)
	return s
end

--- Fixate random weapon
function attachRandomWeapon(cur_ship,weapons)
	if weapons==nil or #weapons==0 then return end
	--Randomly assign a weapon to everyone
	i = math.random(#weapons)
	cur_ship:AddWeapon( weapons[i] )
	cur_ship:AddAmmo( weapons[i],100 )
end

--- List of options
function options()
	Epiar.pause()
	if optionWindow ~= nil then
		closeOptions()
		return
	end
	local height=400
	optionWindow = UI.newWindow( 30,100,220,height,"Options")
	optionTabs = UI.newTabCont( 10, 30, 200, height-100,"Options Tabs")
	optionWindow:add(optionTabs)

	-- Sounds
	soundsTab = UI.newTab("Audio")
	optionTabs:add(soundsTab)

	soundsLabel     = UI.newLabel(20,20,"Sound Options:",0)
	backgroundSound = UI.newCheckbox(20,  30, ( Epiar.getoption("options/sound/background") ), "Background sounds")
	weaponsSound    = UI.newCheckbox(20,  50, ( Epiar.getoption("options/sound/weapons")    ), "Weapons sounds")
	enginesSound    = UI.newCheckbox(20,  70, ( Epiar.getoption("options/sound/engines")    ), "Engines sounds")
	explosionsSound = UI.newCheckbox(20,  90, ( Epiar.getoption("options/sound/explosions") ), "Explosions sounds")
	buttonsSound    = UI.newCheckbox(20, 110, ( Epiar.getoption("options/sound/buttons")    ), "Buttons sounds")
	--[[soundVolume     = { Textbox = UI.newTextbox(20,  150, 30, 1,Epiar.getoption("options/sound/soundvolume")),
						Label = UI.newLabel(55, 165, "Sound volume (0-1)")}
	musicVolume     = { Textbox = UI.newTextbox(20,  170, 30, 1, Epiar.getoption("options/sound/musicvolume")),
						Label = UI.newLabel(55, 185, "Music volume (0-1)")}
						--]]
	soundVolume = { Slider = UI.newSlider(20, 140, 80, 16, "Sound Volume","Audio.setSoundVolume"),
					Label = UI.newLabel( 105, 152, "Sound Volume", 0)}
	musicVolume = { Slider = UI.newSlider(20, 170, 80, 16, "Music Volume","Audio.setMusicVolume"),
					Label = UI.newLabel( 105, 182, "Music Volume", 0)}
	soundsTab:add(  soundsLabel,
					backgroundSound,
					weaponsSound,
					enginesSound,
					explosionsSound,
					buttonsSound,
					soundVolume.Slider,
					soundVolume.Label,
					musicVolume.Slider,
					musicVolume.Label)

	-- Debugging
	debugTab = UI.newTab("Debugging")
	optionTabs:add(debugTab)
	debugLabel      = UI.newLabel(20,200,"Debug Options:",0)
	xmlfileLogging  = UI.newCheckbox(20, 210, ( Epiar.getoption("options/log/xml") ), "Save Log Messages")
	stdoutLogging   = UI.newCheckbox(20, 230, ( Epiar.getoption("options/log/out") ), "Print Log Messages")
	quadTreeDisplay = UI.newCheckbox(20, 250, ( Epiar.getoption("options/development/debug-quadtree") ), "Display QuadTree")
	debugTab:add( debugLabel, xmlfileLogging, stdoutLogging, quadTreeDisplay)
	
	function saveOptions()
		Epiar.setoption("options/sound/background", backgroundSound :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/weapons",    weaponsSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/engines",    enginesSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/explosions", explosionsSound :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/buttons",    buttonsSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/xml",          xmlfileLogging  :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/out",    		stdoutLogging   :IsChecked() and 1 or 0 )
		Epiar.setoption("options/development/debug-quadtree", quadTreeDisplay :IsChecked() and 1 or 0 )
	end
	function closeOptions()
		optionWindow:close();
		optionWindow=nil;
		Epiar.unpause()
	end
	optionWindow:add( UI.newButton(20, height-50, 100, 30,"Customize Keys","chooseKeys()") )
	optionWindow:add( UI.newButton(130, height-50, 60, 30,"Save","saveOptions(); closeOptions()") )
end

-- Execute the current plan of each AI
-- Necessary functions for now.

--- Create Some ships around the planets
function planetTraffic()
	planets = Epiar.planets()
	for p=1,#planets do
		planet = planets[p]
		expectedTraffic = 1* planet:Traffic()
		x,y = planet:GetPosition()
		influence = planet:Influence()
		currentTraffic = #(Epiar.ships(x,y,influence))
		if influence>0 and currentTraffic < expectedTraffic then
			models = planet:GetModels()
			engines = planet:GetEngines()
			weapons = planet:GetWeapons()
			for s=currentTraffic,expectedTraffic do
				createRandomShip(x,y,influence,models,engines,weapons)
			end
		end
	end
end

--- This Closure creates more traffic periodically
function moreTraffic(tickcycle)
	ticks = tickcycle
	function traffic()
		ticks = ticks -1
		if ticks == 0 then
			planetTraffic()
			ticks = tickcycle
		end
	end
	return traffic
end
registerPostStep(moreTraffic(1000))

--- Aim at center
function aimCenter(cur_ship,timeleft)
	-- direction towards the center or the universe
	if timeleft%3 ==0 then
		cur_ship:Rotate( cur_ship:directionTowards(0,0) )
	end
	cur_ship:Fire()
	cur_ship:Accelerate()
end
registerInit(planetTraffic)
registerPlan(aimCenter)

--- Buys a ship
function buyShip(model)
	price = Epiar.getMSRP(model)
	if player_credits >= price then
		currentModel = PLAYER:GetModelName()
		if currentModel ~= model then
			player_credits = player_credits - price + (2/3.0)*(Epiar.getMSRP(currentModel))
			HUD.newAlert("Enjoy your new "..model.." for "..price.." credits.")
			PLAYER:SetModel(model)
			PLAYER:Repair(10000)
		else
			HUD.newAlert("You already have a "..model)
		end
	else
		HUD.newAlert("You can't afford to buy a "..model)
	end
	return 1
end

--- Buys a weapon
function buyWeapon(weapon)
	price = Epiar.getMSRP(weapon)
	if player_credits >= price then
		player_credits = player_credits - price
		weaponsAndAmmo = PLAYER:GetWeapons()
		if weaponsAndAmmo[weapon]~=nil then
			PLAYER:AddAmmo(weapon,100)
			return 0
		else
			HUD.newAlert("Enjoy your new "..weapon.." system for "..price.." credits")
			PLAYER:AddWeapon(weapon)
			PLAYER:AddAmmo(weapon,100)
			myweapons[weapon] = HUD.newStatus(weapon..":",130,0,"[ ".. 100 .." ]")
		end
	else
		HUD.newAlert("You can't afford to buy a "..weapon)
	end
	return 1
end

--- Buys an Engine
function buyEngine(engine)
	price = Epiar.getMSRP(engine)
	if player_credits >= price then
		player_credits = player_credits - price
		HUD.newAlert("Enjoy your new "..engine.." system for "..price.." credits")
		PLAYER:SetEngine(engine)
	else
		HUD.newAlert("You can't afford to buy a "..engine)
	end
	return 1
end

--- Creates a table
function createTable(win,w,h,piclist,buttonlist)
	pad = 10
	box = 120
	button_h = 30
	button_w = 100
	row,col = 1,1
	getPos = function(c,r)
		pos_x = box*(col-1)+20
		pos_y = box*(row-1)+20
		return pos_x,pos_y
	end

	-- Lay out the buttons with pictures beneath them
	for i=1,#piclist do
		pos_x,pos_y = getPos(col,row)

		win:add(UI.newButton( pos_x+(box-button_w)/2+box, pos_y+box/2-button_h/2, button_w,button_h, buttonlist[i][1], buttonlist[i][2]))
		win:add(UI.newPicture( pos_x, pos_y + button_h, box,box-button_h,piclist[i]))

		row=row+1
	end
end

--- Land on a planet
function landingDialog(id)
	-- Create the Planet Landing Screen
	if landingWin ~= nil then return end
	Epiar.pause()
	planet = Epiar.getSprite(id)
	
	height = 400
	width = 300
	landingWin = UI.newWindow( 200,100,width,height, string.format("%s Landing Screen",planet:GetName()))
	storeframe = UI.newTabCont( 10, 30, width-20, height-100,"Store")
	landingWin:add(storeframe)

	-- Shipyard
	shipyard = UI.newTab("Ship Yard")
	local models = planet:GetModels()
	local modelButtons = {}
	for m =1,#models do
		price = Epiar.getMSRP(models[m])
		modelButtons[m] = {models[m]..": "..price, "buyShip(\""..models[m].."\")"}
	end
	createTable(shipyard,width-20,height-100,models,modelButtons)

	-- Armory
	armory = UI.newTab("Armory")
	local weapons = planet:GetWeapons()
	local weaponButtons= {}
	for i =1,#weapons do
		price = Epiar.getMSRP(weapons[i])
		weaponButtons[i] = {weapons[i]..": "..price, "buyWeapon(\""..weapons[i].."\")"}
	end
	createTable(armory,width-20,height-100,weapons,weaponButtons)

	-- Outfitting
	outfitting = UI.newTab("Outfitting")
	local engines = planet:GetEngines()
	local engineButtons= {}
	for i =1,#engines do
		price = Epiar.getMSRP(engines[i])
		engineButtons[i] = {engines[i]..": "..price, "buyEngine(\""..engines[i].."\")"}
	end
	createTable(outfitting,width-20,height-100,engines,engineButtons)

	storeframe:add(shipyard,armory,outfitting)

	landingWin:add(UI.newButton( 10,height-40,100,30,"Repair","PLAYER:Repair(10000)" ))
	landingWin:add(UI.newButton( width-110,height-40,100,30,string.format("Leave %s ",planet:GetName()), "Epiar.unpause();landingWin:close();landingWin=nil" ))
end

--- UI demo
function ui_demo()
	if demo_win ~= nil then return end

	-- Create the widgets
	demo_win = UI.newWindow( 200, 100, 400, 300, "User Interface Demo")
	demo_text1 = UI.newTextbox( 50, 50, 100, 1)
	demo_text2 = UI.newTextbox( 250, 50, 100, 1)
	io.write("DEBUG '"..( Epiar.getoption("options/development/debug-quadtree") ).."'\n")
	demo_check = UI.newCheckbox(50, 100, ( Epiar.getoption("options/development/debug-quadtree") ), "Toggle This")

	-- Modify the Widgets
	demo_trigger = function ()
		Epiar.setoption("options/development/debug-quadtree", demo_check:IsChecked() and 1 or 0 )
		demo_win:close()
		demo_win = nil;
	end
	demo_swap = function()
		s1 = demo_text1:GetText()
		s2 = demo_text2:GetText()
		demo_text1:setText(s2)
		demo_text2:setText(s1)
	end

	-- Attach the widgets to the window
	demo_win:add(demo_text1)
	demo_win:add(demo_text2)
	demo_win:add(demo_check)
	demo_win:add(UI.newButton( 175, 50, 14*3, 18, "<->", "demo_swap()"))
	demo_win:add(UI.newButton( 152, 262-45, 96, 25, "TOGGLE", "demo_check:setChecked(not demo_check:IsChecked() )"))
	demo_win:add(UI.newButton( 152, 262, 96, 25, "OK", "demo_trigger()"))
end


--------------------------------------------------------------------------------
-- Load Scenarios

dofile "Resources/Scripts/basics.lua"
--dofile "Resources/Scripts/tag.lua"
--dofile "Resources/Scripts/swarm.lua"
dofile "Resources/Scripts/commands.lua"

