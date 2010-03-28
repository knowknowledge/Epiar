-- Use this script for code directly to the Players

player_credits = 10000

playerCommands = {
	-- Each command should be a table
	-- { KEY, TITLE, SCRIPT }
	{'up', "Accelerate", "PLAYER:Accelerate()",KEYPRESSED},
	{'left', "Turn Left", "PLAYER:Rotate(30)",KEYPRESSED},
	{'right', "Turn Right", "PLAYER:Rotate(-30)",KEYPRESSED},
	{'down', "Reverse", "PLAYER:Rotate(PLAYER:directionTowards(PLAYER:GetMomentumAngle() + 180 ))",KEYPRESSED},
	{'c', "Center", "PLAYER:Rotate(PLAYER:directionTowards(0,0))",KEYPRESSED},
	{'rshift', "Change Weapon 1", "PLAYER:ChangeWeapon()",KEYTYPED},
	{'lshift', "Change Weapon 2", "PLAYER:ChangeWeapon()",KEYTYPED},
	{'tab', "Target Ship", "targetClosestShip()",KEYTYPED},
	{'l', "Land on Planet", "attemptLanding()",KEYTYPED},
	{'w', "Focus on the Target", "Epiar.focusCamera(HUD.getTarget())",KEYTYPED},
	{'q', "Focus on the Player", "Epiar.focusCamera(PLAYER:GetID())",KEYTYPED},
	{'space', "Fire", "PLAYER:Fire()",KEYPRESSED},
}

function playerStart()
	PLAYER = Epiar.player()
	createHUD()
	registerPostStep(updateHUD)
	registerCommands(playerCommands)
end

--- Target closest ship
function targetClosestShip()
	x,y = PLAYER:GetPosition()
	nearby = Epiar.ships(x,y,4096)
	if #nearby==0 then return end
	
	nextTarget = 1
	currentTarget = HUD.getTarget()
	for s =1,#nearby-1 do
		if nearby[s]:GetID() == currentTarget then
			nextTarget = s+1
		end
	end
	
	HUD.newAlert("Targeting "..nearby[nextTarget]:GetModelName().." #"..nearby[nextTarget]:GetID())
	HUD.setTarget(nearby[nextTarget]:GetID()) -- First ID in the list
	TargetName:setStatus(nearby[nextTarget]:GetModelName() )
end

--- Try to land
function attemptLanding()
	if landingWin ~= nil then return end
	x,y = PLAYER:GetPosition()
	planet = Epiar.nearestPlanet(PLAYER,4096)
	px,py = planet:GetPosition()
	distance = distfrom( px,py, x,y)
	message=""
	if HUD.getTarget() ~= planet:GetID() then -- Add this text before the first message.
		message = string.format("This is %s Landing Control. ",planet:GetName())
	end
	
	-- Check if the ship is close enough and moving slowly enough to land on the planet.
	HUD.setTarget(planet:GetID())
	TargetName:setStatus(planet:GetName() )
	-- TODO make this distance check based off of the planet size.
	if distance > 200 then
		if message~="" then
			message=message.."Begin your approach."
		else
			message="Continue your approach."
		end
		HUD.newAlert(message)
	else
		velocity = PLAYER:GetMomentumSpeed()
		if velocity > 2 then
			HUD.newAlert(message.."Please slow your approach.")
		else
			HUD.newAlert(string.format("Welcome to %s.",planet:GetName()))
			landingDialog( planet:GetID() )
		end
	end
end

--- Register the player functions
function radarZoomKeys()
	for k =1,9 do
		kn = string.byte(k)
		ks = string.format("%d",1000*math.pow(2,k-1))
		Epiar.RegisterKey(kn, KEYPRESSED, "HUD.setVisibity("..ks..")")
	end
end
registerInit(radarZoomKeys)

--- Convert coordinate to quadrant
function coordinateToQuadrant(x,y)
	qsize = 4096
	function c2q(z)
		return math.floor( (z+qsize)/(2*qsize))
	end
	return c2q(x),c2q(y)
end

--- Create a HUD
function createHUD()
	-- Location Status Bars
	x,y = PLAYER:GetPosition()
	qx,qy = coordinateToQuadrant(x,y)
	pos = HUD.newStatus("Coordinate:",130,1,string.format("( %d , %d )",x,y))
	quad = HUD.newStatus("Quadrant:",130,1,string.format("( %d , %d )",qx,qy))
	creditBar = HUD.newStatus("Credits:",130,1,string.format("$%d",player_credits))

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
	TargetHULL = HUD.newStatus("Target:",130,1,0)
	TargetMachine = HUD.newStatus("Machine:",130,1,"")
	TargetState = HUD.newStatus("State:",130,1,0)
end

updateHUD = function ()
	if PLAYER:GetHull() == 0 then return end
	-- Update Positions
	local x,y = PLAYER:GetPosition()
	local qx,qy = coordinateToQuadrant(x,y)
	pos:setStatus(string.format("( %d , %d )",x,y))
	quad:setStatus(string.format("( %d , %d )",qx,qy))
	creditBar:setStatus(string.format("$%d",player_credits))

	-- Update Weapons and Armor
	myhull:setStatus(PLAYER:GetHull())
	local weaponsAndAmmo = PLAYER:GetWeapons()
	local cur_weapon = PLAYER:GetCurrentWeapon()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		if cur_weapon == weapon then star=" ARMED" else star="" end
		if 0==ammo then ammo="---" end
		myweapons[weapon]:setStatus("[ ".. ammo .." ]".. star)
	end
	local target = Epiar.getSprite( HUD.getTarget() )
	if target~=nil then
		if ( target:GetType() == 8) or ( target:GetType() == 16) then
			TargetHULL:setStatus( target:GetHull() )
		end
		if ( target:GetType() == 8) then
			local machine, state = target:GetState()
			TargetMachine:setStatus(machine)
			TargetState:setStatus(state)
		end
	end
end

function loadingWindow()
	if loadingWin~=nil then return end
	Epiar.pause()
	width=300
	height=300
	loadingWin = UI.newWindow( 300,300,width,height,"Welcome to Epiar" )
	local players = Epiar.players()
	for i=1,#players do
		local player = players[i]
		-- TODO: show a preview of the player (curret ship, location, equipment)
		loadingWin:add( UI.newButton(50,30+i*40,100,30,player,string.format("Epiar.loadPlayer('%s'); loadingWin:close(); loadingWin=nil; playerStart(); Epiar.unpause()",player)))
	end
	loadingWin:add( UI.newButton(width/2-50,height-40,100,30,"New Player","createNewPlayerWindow()") )
end
registerInit(loadingWindow)

function createNewPlayerWindow()
	if newPlayerWin~=nil then return end
	newPlayerWin = UI.newWindow( 600,300, 200,200, "New Player")

	-- TODO: show a picture of a the default ship?
	
	yoff=40 -- Buffer for the titlebar
	newPlayerWin:add(UI.newLabel( 10, yoff+15, "Name:"))
	playerNameField = UI.newTextbox( 90, yoff, 100, 1, "")
	newPlayerWin:add(playerNameField)
	yoff = yoff+20

	--newPlayerWin:add(UI.newLabel( 10, yoff+15, "Ship Name"))
	--playerShipField = UI.newTextbox( 90, yoff, 100, 1, "")
	--newPlayerWin:add(playerShipField)
	--yoff = yoff+20

	newPlayerWin:add( UI.newButton( 50,200-40,100,30,"Save", "createNewPlayer()"))
end

function createNewPlayer()
	if newPlayerWin==nil then 
		print( "Oh no! Where did the newPlayerWin go?  We're going to need that!" )
		return
	end
	Epiar.newPlayer(playerNameField:GetText())
	newPlayerWin:close()
	loadingWin:close()
	playerStart();
	Epiar.unpause()
end
