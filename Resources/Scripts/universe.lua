-- Use this script for a solar system
math.randomseed(os.time())

--------------------------------------------------------------------------------
-- Init is a list of functions to be run when the game (re)starts

Init = {}
--- Initialization functions
function registerInit(step)
	table.insert(Init,step)
end

--------------------------------------------------------------------------------
-- Registered Plans for AI to choose from

--------------------------------------------------------------------------------
-- Steps taken during each Update

Steps = {}
--- Steps after each update
function registerStep(step)
	table.insert(Steps,step)
end

--- Run the functions
function Start()
	io.write(string.format("\tInit: %d\n\tSteps: %d\n", #Init, #Steps ))
	for i,func in ipairs(Init) do
		func()
	end
end

--- Update function
function Update()
	if #Steps >0 then
		for i,post_func in ipairs(Steps) do
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
	registerStep(heal)
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

	soundsLabel     = UI.newLabel(20,10,"Sound Options:",0)
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
					Label = UI.newLabel( 105, 140, "Sound Volume", 0)}
	musicVolume = { Slider = UI.newSlider(20, 170, 80, 16, "Music Volume","Audio.setMusicVolume"),
					Label = UI.newLabel( 105, 170, "Music Volume", 0)}
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
	debugLabel      = UI.newLabel(20,10,"Debug Options:",0)
	xmlfileLogging  = UI.newCheckbox(20, 30, ( Epiar.getoption("options/log/xml") ), "Save Log Messages")
	stdoutLogging   = UI.newCheckbox(20, 50, ( Epiar.getoption("options/log/out") ), "Print Log Messages")
	uiLogging       = UI.newCheckbox(20, 70, ( Epiar.getoption("options/log/ui") ), "Save UI as XML")
	spriteLogging   = UI.newCheckbox(20, 90, ( Epiar.getoption("options/log/sprites") ), "Save Sprites as XML")
	quadTreeDisplay = UI.newCheckbox(20,110, ( Epiar.getoption("options/development/debug-quadtree") ), "Display QuadTree")
	aiStateDisplay  = UI.newCheckbox(20,130, ( Epiar.getoption("options/development/debug-ai") ), "Display AI State Machine")
	debugTab:add( debugLabel, xmlfileLogging, stdoutLogging, uiLogging, spriteLogging, quadTreeDisplay, aiStateDisplay)
	
	function saveOptions()
		Epiar.setoption("options/sound/background", backgroundSound :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/weapons",    weaponsSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/engines",    enginesSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/explosions", explosionsSound :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/buttons",    buttonsSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/xml",          xmlfileLogging  :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/out",          stdoutLogging   :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/ui",           uiLogging       :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/sprites",       spriteLogging   :IsChecked() and 1 or 0 )
		Epiar.setoption("options/development/debug-quadtree", quadTreeDisplay :IsChecked() and 1 or 0 )
		Epiar.setoption("options/development/debug-ai", aiStateDisplay :IsChecked() and 1 or 0 )
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
registerStep(moreTraffic(1000))
registerInit(planetTraffic)

--- Buys a ship
function buyShip(model)
	if model==nil then
		model = shipstats["Name"]:GetText()
		print('Buying ',model)
	end
	print('Buying ',model)
	price = Epiar.getMSRP(model)
	player_credits = PLAYER:GetCredits()
	if player_credits >= price then
		currentModel = PLAYER:GetModelName()
		if currentModel ~= model then
			PLAYER:SetCredits( player_credits - price + (2/3.0)*(Epiar.getMSRP(currentModel)) )
			HUD.newAlert("Enjoy your new "..model.." for "..price.." credits.")
			PLAYER:SetModel(model)
			PLAYER:Repair(10000)
		else
			HUD.newAlert("You already have a "..model)
		end
	else
		HUD.newAlert("You can't afford to buy a "..model)
		HUD.newAlert(string.format("You only have %d credits, but you need %d.",player_credits,price))
	end
	return 1
end

function buyOutfit(outfit)
	if outfit==nil then
		outfit = outfitstats["Name"]:GetText()
		print("Buying Outfit ("..outfit..")")
	end
	if Epiar.getWeaponInfo(outfit) then
		print("Weapon...")
		buyWeapon(outfit)
	elseif Epiar.getEngineInfo(outfit) then
		print("Engine...")
		buyEngine(outfit)
	else
		print("Unkown Outfit: "..outfit)
	end
end

--- Buys a weapon
function buyWeapon(weapon)
	price = Epiar.getMSRP(weapon)
	player_credits = PLAYER:GetCredits()
	if player_credits >= price then
		PLAYER:SetCredits( player_credits - price )
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
	player_credits = PLAYER:GetCredits()
	if player_credits >= price then
		PLAYER:SetCredits( player_credits - price )
		HUD.newAlert("Enjoy your new "..engine.." system for "..price.." credits")
		PLAYER:SetEngine(engine)
	else
		HUD.newAlert("You can't afford to buy a "..engine)
	end
	return 1
end

--- Trade a Commodity
function tradeCommodity(transaction, commodity, count, price)
	player_credits = PLAYER:GetCredits()
	cargo,stored,storable = PLAYER:GetCargo()
	print "Trading..."
	if transaction=="buy" then
		print("Tonnage available:",storable-stored)
		print("Tonnage requested:",count)
		print("Tonnage affordable:",player_credits/price)
		trueCount = math.min(storable-stored,count,math.floor(player_credits/price)) -- Can't buy more than this
		stored = PLAYER:StoreCommodities( commodity,trueCount )
		if(stored ~= trueCount) then
			print("ARG! That wasn't supposed to happen!")
		end
		PLAYER:SetCredits( player_credits - trueCount*price )
		HUD.newAlert(string.format("You bought %d tons of %s for %d credits",trueCount,commodity,price*trueCount))
	elseif transaction=="sell" then
		print("Tonnage stored:",cargo[commodity] or 0)
		print("Tonnage requested:",count)
		trueCount = math.min(count,cargo[commodity]) -- Can't sell more than this
		print("Discarding "..trueCount.." Tonnes")
		discarded = PLAYER:DiscardCommodities( commodity,trueCount )
		print("Discarded "..discarded.." Tonnes")
		if(discarded ~= trueCount) then
			print("ARG! That wasn't supposed to happen!")
		end
		PLAYER:SetCredits( player_credits + trueCount*price )
		HUD.newAlert(string.format("You sold %d tons of %s for %d credits",trueCount,commodity,price*trueCount))
	else
		error( string.format( "Sorry, trading Commodities doesn't understand transaction '%s'", transaction ) )
	end
	print "Done Trading..."
	return 1
end

function storeView(storestats, itemType, itemName )
	local getters = {
		["ship"]=Epiar.getModelInfo,
		["weapon"]=Epiar.getWeaponInfo,
		["engine"]=Epiar.getEngineInfo,
	}
	local iteminfo = getters[itemType](itemName)
	print( "viewing "..itemName)
	for infoname,infovalue in pairs(iteminfo) do
		print(infoname,infovalue)
	end
	print('----------')
	for statname,statlabel in pairs(storestats) do
		if iteminfo[statname] == nil then
			iteminfo[statname] = ""
		end
		print(statname, iteminfo[statname] )
		if statname=="Picture"  or statname=="Image" then
			statlabel:setPicture( iteminfo[statname] )
		else
			statlabel:setLabel( iteminfo[statname] )
		end
	end
	print('----------')
end

--- Land on a planet
function landingDialog(id)
	-- Create the Planet Landing Screen
	if landingWin ~= nil then return end
	Epiar.pause()
	planet = Epiar.getSprite(id)
	
	height = 400
	width = 600
	local boxsize=80
	landingWin = UI.newWindow( 200,100,width,height, string.format("%s Landing Screen",planet:GetName()))
	storeframe = UI.newTabCont( 10, 30, width-20, height-100,"Store")
	landingWin:add(storeframe)

	-- Shipyard
	shipyard = UI.newTab("Ship Yard")
	local yoff = 5
	local models = planet:GetModels()
	for i,name in ipairs(models) do
		shipyard:add(UI.newPicture(5,yoff,boxsize,boxsize,name,0,0,0,1))
		yoff = yoff+boxsize
		shipyard:add( UI.newButton( 5,yoff,boxsize,20,name, string.format("storeView(shipstats,'ship','%s')",name)))
		yoff = yoff+30
	end

	--function label_y(num) num*20+10 end
	shipstats = {}
	shipstats["Name"] = UI.newLabel(440, 10+21*(0),"",1)
	shipstats["Image"] = UI.newPicture(120,10,200,200,"",0,0,0,1)
	shipyard:add(shipstats["Name"], shipstats["Image"])
	local statnames = {
		{title= "Hull Strength",statname= "MaxHull"},
		{title= "Shield Strength",statname= "MaxShield"},
		{title= "Mass",statname= "Mass"},
		{title= "Speed",statname= "MaxSpeed"},
		{title= "Cargo Space",statname= "Cargo"},
		{title= "Surface Area",statname= "SurfaceArea"},
		{title= "Rotation Speed",statname= "Rotation"},
		{title= "Cost",statname= "MSRP"},
	}
	for i,stat in ipairs(statnames) do
		yoff = 10+21*(i)
		title = UI.newLabel(340, yoff,stat.title)
		value = UI.newLabel(440, yoff,"")
		shipyard:add(title,value)
		shipstats[stat.statname] = value
	end
	storeView(shipstats,'ship',models[1])
	shipyard:add( UI.newButton( width-200,math.max(210,yoff)+20,100,30,"Buy","buyShip()" ))

	-- Outfitting
	outfitting = UI.newTab("Outfitting")
	local weapons = planet:GetWeapons()
	local engines = planet:GetEngines()
	local yoff = 5
	for i,name in ipairs(weapons) do
		outfitting:add(UI.newPicture(5,yoff,boxsize,boxsize,name,0,0,0,1))
		yoff = yoff+boxsize
		outfitting:add( UI.newButton( 5,yoff,boxsize,20,name, string.format("storeView(outfitstats,'weapon','%s')",name)))
		yoff = yoff+30
	end
	for i,name in ipairs(engines) do
		outfitting:add(UI.newPicture(5,yoff,boxsize,boxsize,name,0,0,0,1))
		yoff = yoff+boxsize
		outfitting:add( UI.newButton( 5,yoff,boxsize,20,name, string.format("storeView(outfitstats,'engine','%s')",name)))
		yoff = yoff+30
	end

	outfitstats = {}
	outfitstats["Name"] = UI.newLabel(440, 10+21*(0),"",1)
	outfitstats["Picture"] = UI.newPicture(120,10,200,200,"",0,0,0,1)
	outfitting:add(outfitstats["Name"], outfitstats["Picture"])
	local statnames = {
		{title= "Payload",statname= "Payload"},
		{title= "Lifetime",statname= "Lifetime"},
		{title= "Fire Delay",statname= "FireDelay"},
		{title= "Tracking",statname= "Tracking"},
		{title= "Force Output",statname= "Force"},
		{title= "Cost",statname= "MSRP"},
	}
	for i,stat in ipairs(statnames) do
		yoff = 10+21*(i)
		title = UI.newLabel(340, yoff,stat.title)
		value = UI.newLabel(440, yoff,"")
		outfitting:add(title,value)
		outfitstats[stat.statname] = value
	end
	if #weapons then
		storeView(outfitstats,'weapon',weapons[1])
	elseif #engines then
		storeView(outfitstats,'engine',engines[1])
	end
	outfitting:add( UI.newButton( width-200,math.max(210,yoff)+20,100,30,"Buy","buyOutfit()" ))

	-- Trade
	trade = UI.newTab("Trade")
	tradeCounts = {} -- This global variable
	local commodities = Epiar.commodities()
	local currentCargo,stored,storable = PLAYER:GetCargo()
	for i,commodity in pairs(commodities) do
		local yoff = 20+i*20
		local msrp = Epiar.getMSRP(commodity)
		local price_offset = math.random(-3,3)
		local priceMeanings = { "(Very Low)","(Low)","","","","(High)","(Very High)" }
		local price = msrp + ( price_offset*msrp/10 )
		print (commodity.."is "..priceMeanings[price_offset+4].." at "..price.." instead of "..msrp)
		local count = 10
		trade:add( UI.newLabel(10,yoff,string.format("%s at %d %s",commodity,price,priceMeanings[price_offset+4]),0) )
		tradeCounts[commodity] = UI.newTextbox(180,yoff,30,1, currentCargo[commodity] or 0)
		trade:add( tradeCounts[commodity] )
		trade:add( UI.newButton(210,yoff,30,20,"Buy",string.format("tradeCommodity('buy','%s',%d,%d)",commodity,count,price )))
		trade:add( UI.newButton(240,yoff,30,20,"Sell",string.format("tradeCommodity('sell','%s',%d,%d)",commodity,count,price )))
	end

	storeframe:add(shipyard,outfitting,trade)

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

