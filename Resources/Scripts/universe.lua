

-- This is the Introduction Message that greets new players.
-- Notice that we use spaces here since Label can't parse tabs.
welcomeMessage = [[
                  Welcome to Epiar

We hope you like it. We're still working on it.


The Epiar controls in a nutshell:
    - Move the ship with the Arrow keys.
    - Land on a Planet by hitting l (little L) when you are near it.

    - Fire with the Spacebar once you buy a gun.
    - To target ships:
          - Hit TAB to cycle through nearby ships.
          - Hit t to select the closest ship.
          - Click on the ship you want to target.
    - To board a disabled target hit B while you are near it.

    - View the map, press m.
    - Change the game options, press ?.
    - Modify the default keyboard controls, press /.
    
    - Your game is saved whenever you land on a planet.
    - Leave to the Main Menu by hitting escape.

For more information about Epiar, please visit:

                  epiar.net

If you have any questions, comments, or bug reports please send us email at:

                  epiar-devel@epiar.net

Thanks for playing!
]]

-- Generate a Random Lua Seed
function randomizeseed()
	math.randomseed(os.time())
	-- Absorb the first few non-random random results
	for s =1,10 do
		math.random();
	end
end

--------------------------------------------------------------------------------
-- Basic Utilities

--- Creates a random ship
function createRandomShip(X,Y,Range,models,engines,weapons,alliance)
	if models==nil then
		local models = Epiar.models()
	end
	if engines==nil then
		local engines = Epiar.engines()
	end
	if weapons==nil then
		local weapons = Epiar.weapons()
	end
	if alliance==nil then
		alliance = choose(Epiar.alliances())
	end
	local name = choose( {
		"Bob", "Joe", "Fred", "Sally", "Frank",
		"Hillary", "Bruce", "Patrick", "Jimbo", "Richard",
		"John", "Chuck", "Arthur", "James", "Bill",
		"Helen", "Ken", "Marcus", "Violet", "Ethel",
		"Gary", "Scott", "Thomas", "Russel", "Steve",
	} )
	local X = X + about(Range)
	local Y = Y + about(Range)
	local model = choose(models)
	local engine = choose(engines)
	local plans = {"Hunter", "Trader", "Patrol", "Bully" }
	local pirateModels = { "Fleet Guard", "Kartanal", "Terran Assist", "Patitu", "Terran Corvert Mark I", "Large Vesper", "Raven", "Hammer Freighter"  }
	local escortModels = { "Fleet Guard", "Terran XV", "Kartanal", "Patitu", "Terran Corvert Mark I"  }

	local p = plans[math.random(#plans)]
	-- Turn some Hunters into anti-player Pirates if the player is far enough along
	if PLAYER:GetCredits() > 10000 and p == "Hunter" and math.random(20) == 1 then p = "Pirate" end
	if p == "Pirate" then
		model = pirateModels[math.random(#pirateModels)]
		engine = "Ion Engines"
	end

	s = Ship.new(name,X,Y,model,engine,p,alliance)

	if p == "Pirate" then
		setHuntHostile(s:GetID(), PLAYER:GetID() )
		local escort = Ship.new("an escort",X-150,Y-150, choose(escortModels), "Ion Engines","Escort",alliance)
		setAccompany(escort:GetID(), s:GetID())
	end

	s:SetRadarColor(255,0,0)

	-- give every AI the standard weapons of their ship class
	attachStandardWeapons(s,weapons)

        local creditsMax = math.random(40,90) * math.sqrt( s:GetTotalCost() )
        local randCredits = math.random( creditsMax )
	s:SetCredits(randCredits)

	return s
end

function attachStandardWeapons(cur_ship,weapons)

	-- first clear the weapon list
	for weap,ammo in pairs( cur_ship:GetWeapons() ) do
		cur_ship:RemoveFromWeaponList(weap)
	end

	-- then populate the weapon list from the standard slot contents
	for slot,weap in pairs( cur_ship:GetWeaponSlotContents() ) do
		cur_ship:AddToWeaponList(weap)
	end
end



function createSystems(seed)
	local alliances = Epiar.alliances()
	local technologies = Epiar.technologies()
	local r = 200000
	local stationGraphic = "Resources/Graphics/Station01.png"
	local planetGraphics = {}
	for p=1,21 do
		table.insert(planetGraphics, "Resources/Graphics/planet"..p..".png" )
	end
	local system = {}

	-- Use a specific Seed to create these Systems
	if seed ~= nil then
		math.randomseed(seed)
	else
		randomizeseed()
	end

	-- Create a bunch of random Star Systems
	local starSystems = {
		"Xen", "Artegga", "Vazzen", "Rilburn", "Burasu",
		"Garor", "Hushaw", "Chenal", "Siana", "Hyanallophos",
		"Allyphos", "Eorith", "Hanacal", "Tyeosur", "Mosalia",
		"Untania", "Tonulia", "Anusia", "Denacia",
	}
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
	table.insert( system, { 
		              ["x"]=0,
		              ["y"]=0,
		              ["alliance"]=alliances[ math.random(#alliances) ],
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
				"Resources/Graphics/planet1s.png",
				"Mostly Harmless",
				choose(technologies)
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
					1,0,0,(30+math.random(100))*100,
					"Resources/Graphics/planet1s.png",
					"Mostly Harmless",
					choose(technologies)
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

function createRandomShipForPlanet(id)
	planet = Epiar.getSprite(id)
	if (planet ~= nil) and (planet:GetType() == SPRITE_PLANET) then
		x,y = planet:GetPosition()
		influence = planet:Influence()
		models = planet:GetModels()
		engines = planet:GetEngines()
		weapons = planet:GetWeapons()
		alliance = planet:GetAlliance()
		createRandomShip(x,y,influence,models,engines,weapons)
	end
end


function intro()
	local height = 550
	local width = 600
	introWin = UI.newWindow( 200, 100, width, height, "Welcome to Epiar")
	
	introWin:add(
		UI.newParagraph( 50, 50, 500, 500, welcomeMessage),
		UI.newButton( width/2-50, height-70, 100, 30, "I'm Ready!", "introComplete()")
	)

	Epiar.pause()
end

function introComplete()
	if UI.search("/Window'Welcome to Epiar'/") == nil then return end
	introWin:close()
	introWin = nil
	-- Unpause if the player loading screen isn't active
	if PLAYER ~= nil then Epiar.unpause() end
end

--- UI demo
function ui_demo()
	if UI.search("/Window'User Interface Demo'/") ~= nil then return end

	-- Create the widgets
	demo_win = UI.newWindow( 200, 100, 400, 300, "User Interface Demo")
	demo_text1 = UI.newTextbox( 50, 50, 100, 1)
	demo_text2 = UI.newTextbox( 250, 50, 100, 1)
	demo_drop = UI.newDropdown( 250, 100, 100, 30, {"A","B","C"})
	io.write("DEBUG '"..( Epiar.getoption("options/development/debug-quadtree") ).."'\n")

	-- Modify the Widgets
	demo_swap = function()
		s1 = demo_text1:GetText()
		s2 = demo_text2:GetText()
		demo_text1:setText(s2)
		demo_text2:setText(s1)
	end

	demo_button = UI.newButton( 175, 50, 14*3, 18, "<->", "demo_swap()")

	-- Attach the widgets to the window
	demo_win:add(demo_text1)
	demo_win:add(demo_text2)
	demo_win:add(demo_button)
	demo_win:add(demo_drop)

	demo_win:setFormButton( demo_button )
end

-- interactive weapon slot configuration
function weaponConfigDialog()
	if UI.search("/Window'Weapon Configuration'/") ~= nil then return end

	local slotCount = PLAYER:GetWeaponSlotCount()

	local height = 60 + (40*slotCount)
	if height < 300 then height = 300 end -- accommodate the instruction text

	local width = 500

	local leftFrame = UI.newFrame( 10, 60, 240, height )
	local rightFrame = UI.newFrame( 250, 60, 240, height )

	local slotPlural = ""
	if slotCount ~= 1 then slotPlural = "s" end
	local shipLabel = UI.newLabel(20, 30, (string.format("Your %s has %d weapon slot%s...", PLAYER:GetModelName(), slotCount, slotPlural)), 0)
	local slotsLabel = UI.newLabel(50, 10, "Weapon slot:", 0)
	local fgLabel = UI.newLabel(150, 10, "Firing group:", 0)

	--local weapLabel = UI.newLabel(25, 5, "Weapons available:", 0)

	local wcInstructions = [[To move a weapon from one slot to another, click on the first slot, then click on the one you want to swap it with.

				There are two firing groups, Primary and Secondary. To switch the firing group of a slot, click on the firing group button.

				Although you may have your own preferences, it is recommended that, if you have a weapon which requires ammo (e.g. missile launcher), you keep it in a firing group separate from your main group.

				Grouping missiles into a salvo is a possibility if you have more than one launcher.]]
	wcInstructions = string.gsub(wcInstructions, "\t*", "")

	local instructions = UI.newParagraph(25, 5, 190, height, wcInstructions )

	local wcDialog = UI.newWindow( 300,50,width,height+30+30+40, "Weapon Configuration")

	wcDialog:add(shipLabel)
	wcDialog:add(leftFrame, rightFrame);
	wcDialog:add(UI.newButton( 200, height+30+30, 100, 30, "Finish", "weaponConfigFinish()"))
	wcDialog:addCloseButton()

	leftFrame:add(slotsLabel, fgLabel);
	rightFrame:add(instructions);

	-- when assignWeaponToSlot() is called, if this is nil, set it;
	-- if it's set, swap the slot contents, update the buttons,
	-- then set it back to nil.
	pickedSlot = nil

	slotButtons = { }
	slotFGButtons = { }

	for slot =0,(slotCount-1) do
		local slotName = PLAYER:GetWeaponSlotName(slot)
		local slotStatus = PLAYER:GetWeaponSlotStatus(slot)
		local slotFG = PLAYER:GetWeaponSlotFG(slot)
		local slotFGName = "Primary"
		if slotFG == 1 then slotFGName = "Secondary" end

		local slotLabel = UI.newLabel( 15, 45+(40*slot), (string.format("%s:", slotName)), 0)
		slotButtons[slot] = UI.newButton( 50, 45+(40*slot)+20, 100, 20, (string.format("%s", slotStatus)), (string.format("assignWeaponToSlot(%d)", slot)))
		slotFGButtons[slot] = UI.newButton( 150, 45+(40*slot)+20, 75, 20, slotFGName, (string.format("alternateFiringGroup(%d)", slot)))
		leftFrame:add(slotLabel,slotButtons[slot],slotFGButtons[slot]);
	end

	local w = 0

end

function assignWeaponToSlot(slot)
	if(pickedSlot == nil) then
		pickedSlot = slot
	else
		-- swap contents of pickedSlot and slot
		local s = PLAYER:GetWeaponSlotStatus(slot)
		PLAYER:SetWeaponSlotStatus(slot, PLAYER:GetWeaponSlotStatus(pickedSlot) )
		PLAYER:SetWeaponSlotStatus(pickedSlot, s)

		-- now they have been swapped, so update the buttons
		slotButtons[slot]:setText( PLAYER:GetWeaponSlotStatus(slot) )
		slotButtons[pickedSlot]:setText( PLAYER:GetWeaponSlotStatus(pickedSlot) )

		pickedSlot = nil
	end
end

function alternateFiringGroup(slot)
	local fg = PLAYER:GetWeaponSlotFG(slot)
	fg = (fg+1)%2
	PLAYER:SetWeaponSlotFG(slot, fg)
	slotFGButtons[slot]:setText( (PLAYER:GetWeaponSlotFG(slot) == 0 and "Primary" or "Secondary") )
end

function weaponConfigFinish()
	-- the slot editing itself took place while the dialog was open, so nothing more needs to be done at this point
	local wcDialog = UI.search("/Window'Weapon Configuration'/")
	-- this should cover all widget cleanup
	if wcDialog ~= nil then
		wcDialog:close()
	end
end
