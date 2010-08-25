-- Use this script for code directly to the Players


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
	{'tab', "Target Ship", "targetShip()", KEYTYPED},
	{'t', "Target Closest Ship", "targetClosestShip()",KEYTYPED},
	{'i', "Player Info", "playerInformation()", KEYTYPED},
	{'l', "Land on Planet", "attemptLanding()", KEYTYPED},
	{'L', "Land on Planet", "attemptLanding()", KEYTYPED},
	{'w', "Focus on the Target", "Epiar.focusCamera(HUD.getTarget())", KEYTYPED},
	{'q', "Focus on the Player", "Epiar.focusCamera(PLAYER:GetID())", KEYTYPED},
	{'space', "Fire", "PLAYER:Fire( HUD.getTarget() )", KEYPRESSED},
	{'b', "Board", "boardShip()", KEYTYPED},
}

function playerStart()
	PLAYER = Epiar.player()
	createHUD()
	registerCommands(playerCommands)
end

--- Target ship
function targetShip()
	local x,y = PLAYER:GetPosition()
	local nearby = Epiar.ships(x,y,4096)
	if #nearby==0 then return end
	
	local nextTarget = 1
	local currentTarget = HUD.getTarget()
	local lastdist = 0

	for s =1,#nearby-1 do
		if nearby[s]:GetID() == currentTarget then
			nextTarget = s+1
		end
	end
	
	HUD.setTarget(nearby[nextTarget]:GetID()) -- First ID in the list
end

---Target closest ship
function targetClosestShip()
	local x,y = PLAYER:GetPosition()
	local nearby = Epiar.ships(x,y,4096)
	if #nearby<=1 then return end
	local shipx, shipy = nearby[1]:GetPosition()
	---print("#nearby=" .. #nearby .. '\n')
	local closest =  { index = 1 , dist = math.sqrt( ( shipx - x) ^ 2 + ( shipy - y) ^ 2) }

	---print( "\nclosest.index= " .. closest.index .. " closest.dist= " .. closest.dist .. '\n')
	for i=2,#nearby do
		shipx, shipy = nearby[i]:GetPosition()
		newDist = math.sqrt (( shipx - x) ^ 2 + ( shipy - y) ^ 2)
		---print("i=" ..i.. " newdist= " .. newDist .. "closest.dist= " .. closest.dist .. '\n')
		if closest.dist > newDist then
			closest.dist = newDist
			closest.index=i
		end		
	end
	nextTarget = closest.index
	HUD.newAlert("Targeting "..nearby[nextTarget]:GetModelName().." #"..nearby[nextTarget]:GetID())
	HUD.setTarget(nearby[nextTarget]:GetID()) -- First ID in the list
end

---Board closest ship if possible
function boardShip()
	if boardingDialog ~= nil then return end -- Abort if the player is already boarding something.

	local x, y = PLAYER:GetPosition()
	local targettedShip = Epiar.getSprite( HUD.getTarget() ) -- acquire target

	if targettedShip == nil then
		HUD.newAlert("Cannot board - no target.")
		return
	end

	local targettedX, targettedY = targettedShip:GetPosition()
	local dist = distfrom( x, y, targettedX, targettedY ) -- calculate distance to target

	if (dist < 200) and (targettedShip:IsDisabled() == 1) then
		HUD.newAlert("Boarding ship...")
		Epiar.pause()

		-- show the boarding dialog
		local moneyOnBoard = targettedShip:GetTotalCost() + targettedShip:GetCredits()
		boardingDialog = UI.newWindow(100, 100, 300, 150, "Boarding Ship")
		boardingDialog:add( UI.newLabel(50, 30, "That was easy!") )
		boardingDialog:add( UI.newButton(50, 80, 200, 30, "Take the Monies!", string.format("doBoarding(%d)", moneyOnBoard ) ) )

	else
		HUD.newAlert("Cannot board target -- too far away")
	end
end

--- Callback for the UI button in boarding ship dialog (see above)
function doBoarding( reward )
	if boardingDialog == nil then return end --- this should never happen

	addcredits( reward )

	Epiar.unpause()
	boardingDialog:close()
	boardingDialog = nil
end

--- Try to land
function attemptLanding()
	if landingWin ~= nil then return end

	local x,y = PLAYER:GetPosition()
	local planet = Epiar.nearestPlanet(PLAYER,4096)
	local px,py = planet:GetPosition()
	local distance = distfrom( px,py, x,y)
	local message = ""

	if HUD.getTarget() ~= planet:GetID() then -- Add this text before the first message.
		message = string.format("This is %s Landing Control. ", planet:GetName())
		
		-- Check if the ship is close enough and moving slowly enough to land on the planet.
		HUD.setTarget(planet:GetID())

		return
	end
	
	-- TODO make this distance check based off of the planet size.
	if distance > 200 then
		if message ~= "" then
			message = message.."Begin your approach."
		else
			message = "Continue your approach."
		end
		HUD.newAlert(message)
	else
		velocity = PLAYER:GetMomentumSpeed()
		if velocity > 2 then
			HUD.newAlert(message.."Please slow your approach.")
		else
			HUD.newAlert(string.format("Welcome to %s.",planet:GetName()))
			landingDialog( planet:GetID() )
			Epiar.setLastPlanet( planet:GetName() )
			Epiar.savePlayer()
		end
	end
end


---Adds to the player's credits
function addcredits( credits )
	--print("adding " .. credits)
	playerCredits=PLAYER:GetCredits( )
	PLAYER:SetCredits( credits + playerCredits )
end

--- Teleport to any location via a new gate
function goto(x,y)
	local px,py = PLAYER:GetPosition()
	Epiar.NewGatePair(x,y,px,py)
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
	HUD.newStatus("Coordinate:",130,1, "string.format('(%d,%d)',PLAYER:GetPosition())")
	HUD.newStatus("Quadrant:",130,1, "string.format('(%d,%d)',coordinateToQuadrant(PLAYER:GetPosition()))")
	HUD.newStatus("Credits:",130,1, "string.format('$%d',PLAYER:GetCredits())")

	-- Weapon and Armor Status Bars
	HUD.newStatus("HULL:",100,0, "PLAYER:GetHull()")
	HUD.newStatus("Shield:",100,0, "PLAYER:GetShield()")
	myweapons = {}
	local weaponsAndAmmo = PLAYER:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		HUD.newStatus(weapon..":",130,0, string.format("playerAmmo('')",weapon))
	end

	-- Target Bars
	HUD.newStatus("Target (N):",130,1, "HudTargetName()")
	HUD.newStatus("Target (H):",130,1, "HudTargetHull()")
	HUD.newStatus("Target (S):",130,1, "HudTargetShield()")
end

function playerAmmo(weaopnName)
	local weaponsAndAmmo = PLAYER:GetWeapons()
	if weaponsAndAmmo[weaopnName] ~= nil then
		return string.format('%d',weaponsAndAmmo[weaopnName])
	else
		return '---'
	end
end

function HudTargetName()
	local targettedSprite = Epiar.getSprite( HUD.getTarget() ) -- acquire target
	if targettedSprite ~= nil then
		local spritetype = targettedSprite:GetType()
		if spritetype == 0x01 then -- Planet
			return targettedSprite:GetName()
		elseif spritetype == 0x02 then -- Gate Bottom
			return "Jump Gate"
		elseif spritetype == 0x04 then -- Projectile
		elseif spritetype == 0x08 then -- Ship
			return targettedSprite:GetModelName()
		elseif spritetype == 0x10 then -- Player
			return "You"
		elseif spritetype == 0x20 then -- Gate Top
			return "Jump Gate"
		elseif spritetype == 0x40 then -- Effect
		else -- Nothing?
		end
	else -- The Sprite doesn't exist
	end
	return ""
end

function HudTargetHull()
	local targettedSprite = Epiar.getSprite( HUD.getTarget() ) -- acquire target
	if targettedSprite ~= nil then
		local spritetype = targettedSprite:GetType()
		if (spritetype == 0x08) or (spritetype == 0x10) then -- Ship or Player
			return targettedSprite:GetHull()
		end
	end
	return 0
end

function HudTargetShield()
	local targettedSprite = Epiar.getSprite( HUD.getTarget() ) -- acquire target
	if targettedSprite ~= nil then
		local spritetype = targettedSprite:GetType()
		if (spritetype == 0x08) or (spritetype == 0x10) then -- Ship or Player
			return targettedSprite:GetShield()
		end
	end
	return 0
end

function loadingWindow()
	if loadingWin~=nil then return end
	Epiar.pause()
	local width=300
	local height=300
	local players = Epiar.players()

	loadingWin = UI.newWindow( 300,300,width,height,"Load a Player" )

	--- Load an old Player
	yoff = 30
	if #players > 0 then
		loadingWin:add( UI.newLabel( 30, yoff, "Load one of your old saved games:" ) )
		yoff = yoff + 30
		for i=1,#players do
			local player = players[i]
			-- TODO: show a preview of the player (curret ship, location, equipment)
			loadingWin:add( UI.newButton(width/2-50,yoff,100,30,player,string.format("loadPlayer('%s')",player)))
			yoff = yoff + 40
		end
		yoff = yoff + 30
	end

	--- Create a new Player
	loadingWin:add( UI.newLabel( 30, yoff, "Create a new Player:" ) )
	yoff = yoff + 30
	loadingWin:add(UI.newLabel( 50, yoff, "Name:"))
	playerNameField = UI.newTextbox( 100, yoff, 100, 1, "")
	loadingWin:add(playerNameField)
	yoff = yoff + 30
	loadingWin:add( UI.newButton( width/2-50,yoff,100,30,"Create", "createNewPlayer()"))
end

function loadPlayer(playerName)
	Epiar.loadPlayer(playerName)
	if loadingWin~=nil then
		loadingWin:close()
		loadingWin=nil
	end
	playerStart()
	Epiar.unpause()
end

function createNewPlayer()
	Epiar.newPlayer(playerNameField:GetText())
	loadingWin:close()
	playerStart();
	Epiar.unpause()
end

function playerInformation()
	if infoWin~=nil then
		infoWin:close()
		infoWin = nil
		return
	end
	local height = 500
	local width = 300
	infoWin = UI.newWindow( 600,200, width,height, "Player Info")
	local y = 30

	local model = PLAYER:GetModelName()
	local engine = PLAYER:GetEngine()
	local credits = PLAYER:GetCredits()
	infoWin:add( UI.newLabel(20, y, "Model:    ".. model) )
	local y = y+20
	infoWin:add( UI.newLabel(20, y, "Engine:   ".. engine) )
	local y = y+20
	infoWin:add( UI.newLabel(20, y, "Credits:   ".. credits) )
	local y = y+20
	infoWin:add( UI.newPicture( 20, y, width-40,100, model ,0,0,0,1) )
	local y = y+100

	y = y+40
	infoWin:add( UI.newLabel(20, y, "Weapons:") )
	local weaponsAndAmmo = PLAYER:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		y = y+20
		infoWin:add( UI.newLabel(30, y, weapon) )
		infoWin:add( UI.newLabel(230, y, ammo) )
	end

	y = y+40
	infoWin:add( UI.newLabel(20, y, "Outfit:") )
	local outfits = PLAYER:GetOutfits()
	for i,outfit in pairs(outfits) do
		y = y+20
		infoWin:add( UI.newLabel(30, y, outfit) )
	end

	y = y+40
	infoWin:add( UI.newLabel(20, y, "Cargo:") )
	local cargo = PLAYER:GetCargo()
	for cargoName,cargoAmount in pairs(cargo) do
		y = y+20
		infoWin:add( UI.newLabel(30, y, cargoName) )
		infoWin:add( UI.newLabel(230, y, cargoAmount) )
	end
end
