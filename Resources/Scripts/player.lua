-- Use this script for a solar system


-- Keyboard States:
KEYUP, KEYDOWN, KEYPRESSED, KEYTYPED = 0,1,2,3
-- SDL Key Translations
Key = {}
Key['UP']=17;
Key['DOWN']=18;
Key['RIGHT']=19;
Key['LEFT']=20;
Key['ESC']=27;
Key['RSHIFT']=47;
Key['LSHIFT']=48;
Key['SPACE']=32;
Key['TAB']=9;
-- TODO finish this table...

function sdlkey(k)
	if Key[k] then
		return Key[k]
	else
		return k
	end
end

commands = {
	-- Each command should be a table
	-- { KEY, TITLE, SCRIPT }
	{'UP', "Accelerate", "PLAYER:Accelerate()",KEYPRESSED},
	{'LEFT', "Turn Left", "PLAYER:Rotate(30)",KEYPRESSED},
	{'RIGHT', "Turn Right", "PLAYER:Rotate(-30)",KEYPRESSED},
	{'DOWN', "Reverse", "PLAYER:Rotate(PLAYER:directionTowards(PLAYER:GetMomentumAngle() + 180 ))",KEYPRESSED},
	{'c', "Center", "PLAYER:Rotate(PLAYER:directionTowards(0,0))",KEYPRESSED},
	{'TAB', "Change Weapon 1", "PLAYER:ChangeWeapon()",KEYTYPED},
	{'LSHIFT', "Change Weapon 2", "PLAYER:ChangeWeapon()",KEYTYPED},
	{'t', "Target Ship", "targetClosestShip()",KEYTYPED},
	{'l', "Target Planet", "targetClosestPlanet()",KEYTYPED},
	{'SPACE', "Fire", "PLAYER:Fire()",KEYPRESSED}
}

function chooseKeys()
	Epiar.pause()
	if keywin ~=nil then return end
	-- 
	off_x,off_y = 30,30
	w,h = 300,(#commands*20 + 80)
	keyinput = {}
	labels = {}
	-- 
	keywin = UI.newWindow( 400, 100, w, h, "Keyboard commands")
	for i=1,#commands do
		keyinput[i] = UI.newTextbox(off_x,off_y,70,1)
		keyinput[i]:setText(commands[i][1])
		labels[i] = UI.newLabel(off_x+100,off_y+15,commands[i][2])
		off_y = off_y +20
		keywin:add(keyinput[i])
		keywin:add(labels[i])
	end
	if keyhelpwin ~=nil then return end
	keywin:add(UI.newButton(w/2-120/2, h-38, 120, 30, "Save Changes", "processKeyWin(commands); Epiar.unpause(); keywin:close(); keywin=nil; keyhelpwin:close(); keyhelpwin=nil;"))
	keywin:add(UI.newButton(10, h-38, 30, 30, "?", "keyhelp()"))
end

function processKeyWin(commands)
	for i=1,#commands do
		keyval = keyinput[i]:GetText()
		if keyval ~= commands[i][1] then
			Epiar.UnRegisterKey(sdlkey(commands[i][1]), commands[i][4])
			Epiar.RegisterKey(sdlkey(keyval), commands[i][4], commands[i][3])
			HUD.newAlert(string.format("Registered '%s' to %s", keyval, commands[i][2]))
			commands[i][1] = keyinput[i]:GetText()
		end
	end
end

function keyhelp()
	Epiar.pause()
	if keyhelpwin ~=nil then return end
	off_x,off_y = 20,20
	w,h = 130,50
	-- This seems to be the only way to count keys in a dictionary.
	for k,v in pairs(Key) do h=h+20 end 
	-- TODO: Make this a scrollable window
	keyhelpwin = UI.newWindow( 100, 100, w, h, "Possible Keys")
	for k,v in pairs(Key) do
		off_y = off_y + 20
		keyhelpwin:add( UI.newLabel(off_x,off_y,"- "..k) )
	end
end

function targetClosestShip()
	x,y = PLAYER:GetPosition()
	nearby = Epiar.ships(x,y,2000)
	if #nearby==0 then return end
	HUD.newAlert("Selecting the closest Ship: "..nearby[1]:GetID().." a "..nearby[1]:GetModelName().."\n")
	HUD.setTarget(nearby[1]:GetID()) -- First ID in the list
	TargetName:setStatus(nearby[1]:GetModelName() )
end

function targetClosestPlanet()
	x,y = PLAYER:GetPosition()
	nearby = Epiar.planets(x,y,2000)
	if #nearby==0 then return end
	HUD.newAlert("Selecting the closest Planet: "..nearby[1]:Name().."\n")
	HUD.setTarget(nearby[1]:GetID()) -- First ID in the list
	TargetName:setStatus(nearby[1]:GetModelName() )
end

function createWindows()
	Epiar.RegisterKey('s',KEYTYPED,"store()")
	Epiar.RegisterKey('p',KEYTYPED,"togglePause()")
	Epiar.RegisterKey('g',KEYTYPED,"ui_demo()")
	Epiar.RegisterKey('k', KEYTYPED, "chooseKeys()" )
	-- pause should 1) not be implemented in lua and 2) should respond to keytyped events, not keydown events, else
	-- a 'p' typed into the UI will also pause the game. this makes no sense. however, if a UI text input has no
	-- focus, the UI will pass the typed event down the chain and pause should reach it eventually
end
registerInit(createWindows)

-- Register the player functions
function createNavigation()
	for i=1,#commands do
		keyval, name, code = commands[i][1],commands[i][2],commands[i][3]
		Epiar.RegisterKey(sdlkey(keyval), KEYPRESSED, code)
	end

	for k =1,9 do
		kn = string.byte(k)
		ks = string.format("%d",k*1000)
		Epiar.RegisterKey(kn, KEYPRESSED, "HUD.setVisibity("..ks..")")
	end
end
registerInit(createNavigation)

function addWeapons()
	weapons = Epiar.weapons()
	for i=1,#weapons do
		PLAYER:AddWeapon( weapons[i] )
		PLAYER:AddAmmo( weapons[i], 100)
	end
end
registerInit(addWeapons)

function coordinateToQuadrant(x,y)
	qsize = 4096
	function c2q(z)
		return math.floor( (z+qsize)/(2*qsize))
	end
	return c2q(x),c2q(y)
end

function createHUD()
	-- Location Status Bars
	x,y = PLAYER:GetPosition()
	qx,qy = coordinateToQuadrant(x,y)
	pos = HUD.newStatus("Coordinate:",130,1,string.format("( %d , %d )",x,y))
	quad = HUD.newStatus("Quadrant:",130,1,string.format("( %d , %d )",qx,qy))

	-- Weapon and Armor Status Bars
	myhull = HUD.newStatus("HULL:",100,0,1.0)
	myweapons = {}
	weaponsAndAmmo = PLAYER:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		if 0==ammo then ammo="---" end
		myweapons[weapon] = HUD.newStatus(weapon..":",130,0,"[ ".. ammo .." ]")
	end

	-- DEBUG Bars
	TargetName = HUD.newStatus("Target:",130,1,"")
end
registerInit(createHUD)

updateHUD = function ()
	-- Update Positions
	x,y = PLAYER:GetPosition()
	qx,qy = coordinateToQuadrant(x,y)
	pos:setStatus(string.format("( %d , %d )",x,y))
	quad:setStatus(string.format("( %d , %d )",qx,qy))

	-- Update Weapons and Armor
	myhull:setStatus(PLAYER:GetHull())
	weaponsAndAmmo = PLAYER:GetWeapons()
	cur_weapon = PLAYER:GetCurrentWeapon()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		if cur_weapon == weapon then star=" ARMED" else star="" end
		if 0==ammo then ammo="---" end
		myweapons[weapon]:setStatus("[ ".. ammo .." ]".. star)
	end
end
registerPostStep(updateHUD)

