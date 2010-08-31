-- Use this script for a solar system
math.randomseed(os.time())

-- This is the Introduction Message that greets new players.
-- Notice that we use spaces here since Label can't parse tabs.
welcomeMessage = [[
                                                  Welcome to Epiar

We hope you like it. We're still working on it.


The Epiar controls in a nutshell:
    - Move the ship with the Arrow keys.
    - Land on a Planet by hitting L when you are near it.

    - Fire with the Spacebar once you buy a gun.
    - To target ships
          - Hit TAB to cycle through nearby ships.
          - Hit t (select the closest ship
          - Click on the ship.
    - To board a disabled target hit B while you are near it.

    - View the map by hitting m or M.
    - To change the keyboard layout or volume controls, press ?.

    - Quit by hitting escape.

For more information about Epiar, please visit:

                                                  epiar.net

If you have any questions, comments, or bug reports please send us email at:

                                                  epiar-devel@epiar.net

Thanks for playing!
]]

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
	--io.write(string.format("\tInit: %d\n\tSteps: %d\n", #Init, #Steps ))
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

function toggleQuadrantMap()
	local currentMap = HUD.getMapDisplay()
	if( currentMap=="QUADRANT") then
		HUD.setMapDisplay("NONE")
	else
		HUD.setMapDisplay("QUADRANT")
	end
end
function toggleUniverseMap()
	local currentMap = HUD.getMapDisplay()
	if( currentMap=="UNIVERSE") then
		HUD.setMapDisplay("NONE")
	else
		HUD.setMapDisplay("UNIVERSE")
	end
end

--- For debugging
function godmode()
	function heal()
		PLAYER:Repair(10000)
	end
	registerStep(heal)
end
--godmode() -- Uncomment this line to never die

--- Convert a list of strings/numbers into an table with those values as keys
-- Code from: http://www.lua.org/pil/11.5.html
function Set (list)
  local set = {}
  for _, l in ipairs(list) do set[l] = true end
  return set
end

--- Trim a string
function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end


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

--- Create a FailureWindow
function NewFailureWindow(Title,Message)
	if FailureWindow ~= nil then return end
	local height = 100
	local width = 300
	FailureWindow= UI.newWindow(350, 350, width, height, Title,
		UI.newLabel(20,20,Message),
		UI.newButton(width/2-50, height-50, 100, 30, "OK", "FailureWindow:close(); FailureWindow = nil"))
end

--- Creates a new ship
function createShip(X,Y,model,engine)
	plans = {"Hunter","Trader","Patrol","Bully"}
	cur_ship = Ship.new(X,Y,model,engine,plans[math.random(#plans)])
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
	local width=300
	local height=400
	local tabwidth=width-20
	local tabheight=height-100
	optionWindow = UI.newWindow( 30,100,width,height,"Options")
	optionTabs = UI.newTabCont( 10, 30, tabwidth, tabheight,"Options Tabs")
	optionWindow:add(optionTabs)

	-- General Game Options
	-- ( No developer stuff here. )
	gameTab = UI.newTab("Game")
	optionTabs:add(gameTab)
	gameLabel      = UI.newLabel(20, 5, "Game Options:", 0)
	fullScreen      = UI.newCheckbox(20,  30, ( Epiar.getoption("options/video/fullscreen") ), "Run as Full Screen")
	showIntro       = UI.newCheckbox(20,  50, ( Epiar.getoption("options/simulation/intro") ), "Show Intro Screen")
	randomUniverse  = UI.newCheckbox(20,  70, ( Epiar.getoption("options/simulation/random-universe") ), "Create a Random Universe")
	autoLoad        = UI.newCheckbox(20,  90, ( Epiar.getoption("options/simulation/automatic-load") ), "Automatically Load the last Player")
	editorMode      = UI.newCheckbox(20, 110, ( Epiar.getoption("options/development/editor-mode") ), "Run in Editor Mode")
	gameTab:add( gameLabel, fullScreen, showIntro, randomUniverse, autoLoad, editorMode )

	-- Sounds
	soundsTab = UI.newTab("Audio")
	optionTabs:add(soundsTab)

	soundsLabel     = UI.newLabel(20, 5, "Sound Options:", 0)
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
	debugTab = UI.newTab("Developer")
	optionTabs:add(debugTab)
	debugLabel      = UI.newLabel(20, 5, "Debug Options:", 0)
	xmlfileLogging  = UI.newCheckbox(20, 30, ( Epiar.getoption("options/log/xml") ), "Save Log Messages")
	stdoutLogging   = UI.newCheckbox(20, 50, ( Epiar.getoption("options/log/out") ), "Print Log Messages")
	uiLogging       = UI.newCheckbox(20, 70, ( Epiar.getoption("options/log/ui") ), "Save UI as XML")
	spriteLogging   = UI.newCheckbox(20, 90, ( Epiar.getoption("options/log/sprites") ), "Save Sprites as XML")
	aiStateDisplay  = UI.newCheckbox(20,110, ( Epiar.getoption("options/development/debug-ai") ), "Display AI State Machine")
	uiDebugging     = UI.newCheckbox(20,130, ( Epiar.getoption("options/development/debug-ui") ), "Display UI Debug Information")
	spritesMap      = UI.newCheckbox(20,150, ( Epiar.getoption("options/development/ships-worldmap") ), "Display Ships on the Universe Map")
	debugTab:add( debugLabel, xmlfileLogging, stdoutLogging, uiLogging, spriteLogging, aiStateDisplay, uiDebugging, spritesMap )

	-- Command Keys
	keyTab = UI.newTab( "Keyboard")
	optionTabs:add(keyTab)
	keyLabel = UI.newLabel(20, 5, "Keyboard Options:", 0)
	keyTab:add(keyLabel)
	local off_x,off_y = 20,30
	keyinput = {} -- Global. We'll need this later.
	labels = {}
	for i=1,#commands do
		local key, name = commands[i][1], commands[i][2]
		keyinput[name] = UI.newTextbox(off_x,off_y,70,1)
		keyinput[name]:setText(key)
		labels[name] = UI.newLabel(off_x+80,off_y-3,name)
		off_y = off_y +20
		keyTab:add(keyinput[name])
		keyTab:add(labels[name])
	end

	function saveOptions()
		-- General Game Options
		Epiar.setoption("options/video/fullscreen",           fullScreen      :IsChecked() and 1 or 0 )
		Epiar.setoption("options/simulation/intro" ,     showIntro       :IsChecked() and 1 or 0 )
		Epiar.setoption("options/simulation/random-universe", randomUniverse  :IsChecked() and 1 or 0 )
		Epiar.setoption("options/simulation/automatic-load",  autoLoad        :IsChecked() and 1 or 0 )
		Epiar.setoption("options/development/editor-mode",    editorMode      :IsChecked() and 1 or 0 )

		-- Sound Options
		Epiar.setoption("options/sound/background", backgroundSound :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/weapons",    weaponsSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/engines",    enginesSound    :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/explosions", explosionsSound :IsChecked() and 1 or 0 )
		Epiar.setoption("options/sound/buttons",    buttonsSound    :IsChecked() and 1 or 0 )

		-- Developer Options
		Epiar.setoption("options/log/xml",          xmlfileLogging  :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/out",          stdoutLogging   :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/ui",           uiLogging       :IsChecked() and 1 or 0 )
		Epiar.setoption("options/log/sprites",      spriteLogging   :IsChecked() and 1 or 0 )
		Epiar.setoption("options/development/debug-ai", aiStateDisplay :IsChecked() and 1 or 0 )
		Epiar.setoption("options/development/debug-ui", uiDebugging :IsChecked() and 1 or 0 )
		Epiar.setoption("options/development/ships-worldmap", spritesMap :IsChecked() and 1 or 0 )

		-- Keyboard Options
		for i=1,#commands do
			local oldkey, name = commands[i][1], commands[i][2]
			if keyinput[name] ~= nil then
				newkey = keyinput[name]:GetText()
				if newkey ~= oldkey then
					Epiar.UnRegisterKey(sdlkey(oldkey), commands[i][4])
					Epiar.RegisterKey(sdlkey(newkey), commands[i][4], commands[i][3])
					HUD.newAlert(string.format("Registered '%s' to %s", newkey, name))
					commands[i][1] = keyinput[name]:GetText()
				end
			end
		end
	end
	function closeOptions()
		optionWindow:close();
		optionWindow=nil;
		Epiar.unpause()
	end
	optionWindow:add( UI.newButton(130, height-50, 60, 30,"Save","saveOptions(); closeOptions()") )
end

function about(r) return math.random(r)-r/2 end

function createSystems()
	local alliances = Epiar.alliances()
	local technologies = Epiar.technologies()
	local r = 200000
	local stationGraphic = "Resources/Graphics/station1.png"
	local planetGraphics = {}
	for p=1,21 do
		table.insert(planetGraphics, "Resources/Graphics/planet"..p..".png" )
	end
	local starSystems = {
		"Xen", "Artegga", "Vazzen", "Rilburn", "Burasu",
		"Garor", "Hushaw", "Chenal", "Siana", "Hyanallophos",
		"Allyphos", "Eorith", "Hanacal", "Tyeosur", "Mosalia",
		"Untania", "Tonulia", "Anusia", "Denacia",
	}
	local system = {}
	for s=1,#starSystems do
		local x = about(r)
		local y = about(r)
		local alliance = alliances[ math.random(#alliances) ]
		local numPlanets = math.random(3)
		local numStations = math.random(3) -1
		local numGates = math.random(3)-1
		system[s] = { ["x"]=x,
		              ["y"]=y,
		              ["alliance"]=alliance,
		              ["numPlanets"]=numPlanets,
		              ["numStations"]=numStations,
		              ["numGates"]=numGates}
	end

	-- Create a system at 0,0 so that new players are attached to the gate grid
	table.insert( system, { ["x"]=0,
		              ["y"]=0,
		              ["alliance"]=alliances[1],
		              ["numPlanets"]=0,
		              ["numStations"]=0,
		              ["numGates"]=0})
	for g=1,3 do
		local other = math.random(#starSystems)
		if other==systemNum then
			other=other+((other==#starSystems) and 1 or -1 )
		end
		local otherSystem = system[ other ]
		local gx = about(5000)
		local gy = about(5000)
		local ox = otherSystem.x + about(5000)
		local oy = otherSystem.y + about(5000)
		Epiar.NewGatePair(gx,gy,ox,oy)
	end

	for systemNum= 1,#starSystems do
		local systemName = starSystems[systemNum]
		local s = system[systemNum]

		-- Create the Planets
		for p=1,s.numPlanets do
			local px = s.x + about(30000)
			local py = s.y + about(30000)
			local name = systemName .." "..p
			Planet.NewPlanet(
				name,
				px,py,
				planetGraphics[ math.random(#planetGraphics) ],
				s.alliance,
				1,math.random(3)-1,math.random(3)-1,math.random(10)*1000,
				technologies[ math.random(#technologies) ]
				)
		end

		-- Create the Stations
		if s.numStations>0 then
			for n=1,s.numStations do
				local px = s.x + about(10000)
				local py = s.y + about(10000)
				local name = systemName .." Outpost "..n
				Planet.NewPlanet(
					name,
					px,py,
					stationGraphic,
					s.alliance,
					1,0,0,math.random(100)*100,
					technologies[ math.random(#technologies) ]
					)
			end
		end

		-- Create the Gates
		if s.numGates>0 then
			for g=1,s.numGates do
				local otherSystem = system[ math.random(#starSystems) ]
				local gx = s.x + about(10000)
				local gy = s.y + about(10000)
				local ox = otherSystem.x + about(10000)
				local oy = otherSystem.y + about(10000)
				Epiar.NewGatePair(gx,gy,ox,oy)
			end
		end
	end
end
--registerInit(createSystems)

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
	local price = Epiar.getMSRP(outfit)
	local player_credits = PLAYER:GetCredits()
	if player_credits < price then
		print("Account overdrawn...")
		HUD.newAlert("You can't afford to buy a "..outfit)
		return
	end

	print("Debiting your account...")

	PLAYER:SetCredits( player_credits - price )
	HUD.newAlert("Enjoy your new "..outfit.." system for "..price.." credits")

	print("Installing Outfit...")
	
	if ( Set(Epiar.weapons())[outfit] ) then
		print("Weapon...")
		local weaponsAndAmmo = PLAYER:GetWeapons()
		if weaponsAndAmmo[outfit]==nil then
			PLAYER:AddWeapon(outfit)
			HUD.newStatus(outfit..":",130,0, string.format("playerAmmo('%s')",outfit))
		end
		local weaponInfo = Epiar.getWeaponInfo(outfit)
		if weaponInfo["Ammo Consumption"] ~= 0 then
			PLAYER:AddAmmo(outfit,100)
		end
	elseif ( Set(Epiar.engines())[outfit] ) then
		print("Engine...")
		PLAYER:SetEngine(outfit)
	elseif ( Set(Epiar.outfits())[outfit] ) then
		print("Outfit...")
		PLAYER:AddOutfit(outfit)
	else
		print("Unknown Outfit: "..outfit)
	end
	print("Outfit Purchase complete")
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
		["outfit"]=Epiar.getOutfitInfo,
	}
	local iteminfo = getters[itemType](itemName)
	--print( "viewing "..itemName)
	--for infoname,infovalue in pairs(iteminfo) do
	--	print(infoname,infovalue)
	--end
	--print('----------')
	for statname,statlabel in pairs(storestats) do
		if iteminfo[statname] == nil then
			iteminfo[statname] = ""
		end
		--print(statname, iteminfo[statname] )
		if statname=="Picture"  or statname=="Image" then
			statlabel:setPicture( iteminfo[statname] )
		else
			value = iteminfo[statname]
			if type(value)=="number" and math.floor(value) ~= value then
				value = string.format("%.2f", value )
			end
			statlabel:setLabel( value )
		end
	end
	--print('----------')
end

--- Land on a planet
function landingDialog(id)
	-- Create the Planet Landing Screen
	if landingWin ~= nil then return end

	Epiar.pause()
	planet = Epiar.getSprite(id)
	
	local height = 500
	local width = 600
	local boxsize = 80
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
	local outfits = planet:GetOutfits()
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
	for i,name in ipairs(outfits) do
		outfitting:add(UI.newPicture(5,yoff,boxsize,boxsize,name,0,0,0,1))
		yoff = yoff+boxsize
		outfitting:add( UI.newButton( 5,yoff,boxsize,20,name, string.format("storeView(outfitstats,'outfit','%s')",name)))
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
		{title= "Speed",statname= "MaxSpeed"},
		{title= "Force Output",statname= "Force"},
		{title= "Rotation Speed",statname= "Rotation"},
		{title= "Hull",statname= "MaxHull"},
		{title= "Shield",statname= "MaxShield"},
		{title= "Mass",statname= "Mass"},
		{title= "Cargo Space",statname= "Cargo"},
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
	elseif #outfits then
		storeView(outfitstats,'outfit',outfits[1])
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
		--print (commodity.."is "..priceMeanings[price_offset+4].." at "..price.." instead of "..msrp)
		local count = 10
		trade:add( UI.newLabel(10,yoff,string.format("%s at %d %s",commodity,price,priceMeanings[price_offset+4]),0) )
		tradeCounts[commodity] = UI.newTextbox(180,yoff,30,1, currentCargo[commodity] or 0)
		trade:add( tradeCounts[commodity] )
		trade:add( UI.newButton(210,yoff,30,20,"Buy",string.format("tradeCommodity('buy','%s',%d,%d)",commodity,count,price )))
		trade:add( UI.newButton(240,yoff,30,20,"Sell",string.format("tradeCommodity('sell','%s',%d,%d)",commodity,count,price )))
	end

	storeframe:add(shipyard,outfitting,trade)

	landingWin:add(UI.newButton( 10,height-40,100,30,"Repair","PLAYER:Repair(10000)" ))
	landingWin:add(UI.newButton( width-110,height-40,100,30,string.format("Leave %s ",planet:GetName()), "Epiar.savePlayer();Epiar.unpause();landingWin:close();landingWin=nil" ))
end

function intro()
	local height = 550
	local width = 600
	introWin = UI.newWindow( 200, 100, width, height, "Welcome to Epiar")
	neverShowAgain = UI.newCheckbox(width/2-90, height-30, 1, "Don't show me this intro again.")
	
	introWin:add(
		UI.newLabel( 50, 50, welcomeMessage),
		neverShowAgain,
		UI.newButton( width/2-50, height-70, 100, 30, "I'm Ready!", "introComplete()")
	)

	Epiar.pause()
end

function introComplete()
	if introWin == nil then return end
	Epiar.setoption("options/simulation/intro", neverShowAgain:IsChecked() and 0 or 1 )
	introWin:close()
	introWin = nil
	-- Unpause if the player loading screen isn't active
	if PLAYER ~= nil then Epiar.unpause() end
end

--- UI demo
function ui_demo()
	if demo_win ~= nil then return end

	-- Create the widgets
	demo_win = UI.newWindow( 200, 100, 400, 300, "User Interface Demo")
	demo_text1 = UI.newTextbox( 50, 50, 100, 1)
	demo_text2 = UI.newTextbox( 250, 50, 100, 1)
	io.write("DEBUG '"..( Epiar.getoption("options/development/debug-quadtree") ).."'\n")

	-- Modify the Widgets
	demo_swap = function()
		s1 = demo_text1:GetText()
		s2 = demo_text2:GetText()
		demo_text1:setText(s2)
		demo_text2:setText(s1)
	end

	-- Attach the widgets to the window
	demo_win:add(demo_text1)
	demo_win:add(demo_text2)
	demo_win:add(UI.newButton( 175, 50, 14*3, 18, "<->", "demo_swap()"))

	demo_win:add(UI.newFrame( 10, 10, 100, 80 ) )
end
