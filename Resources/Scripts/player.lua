-- Use this script for code directly to the Players


--- Keyboard Commands for controlling the Player
-- General commands belong in the defaultCommands table.
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
	{'space', "Fire", "playerFire()", KEYPRESSED},
	{'b', "Board", "boardShip()", KEYTYPED},
	{'y', "Hail", "hailSprite()", KEYTYPED},
	{'s', "Increase Shields", "changePower(1,-0.5,-0.5)", KEYTYPED},
	{'d', "Increase Power", "changePower(-0.5,1,-0.5)", KEYTYPED},
	{'a', "Increase Engine Power", "changePower(-0.5,-0.5,1)", KEYTYPED},
	{'x', "Decrease Shields", "changePower(-1,0.5,0.5)", KEYTYPED},
	{'c', "Decrease Power", "changePower(0.5,-1,0.5)", KEYTYPED},
	{'z', "Decrease Engine Power", "changePower(0.5,0.5,-1)", KEYTYPED},
	{'P', "Open Power Management Window", "powerManagement()", KEYTYPED}
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

powerSlider, shieldSlider, engineSlider = 0, 0, 0

function powerManagement()
	if powerManagementWindow ~=nil then
		powerManagementWindow:close()
		powerManagementWindow=nil
		return
	end
	local width= 200
	local height= 120
	powerManagementWindow = UI.newWindow(WIDTH/2 - width/2 ,HEIGHT - height, width, height, "Power Management")
	
	powerSlider= UI.newSlider(20, 40, 80, 16, "Attack Power", PLAYER:GetDamageBooster()/3.0, "powerShift")
	powerLabel=UI.newLabel(105, 35,"Attack Power",0)
	engineSlider =UI.newSlider(20, 65, 80, 16, "Engine", PLAYER:GetEngineBooster()/3.0, "engineShift")
	engineLabel=UI.newLabel(105, 60,"Engine Power",0)
	shieldSlider = UI.newSlider(20, 85, 80, 16, "Shields", PLAYER:GetShieldBooster()/3.0, "shieldShift")		
	shieldLabel=UI.newLabel(105, 80,"Shield Strength",0)
	
	powerManagementWindow:add(powerSlider, powerLabel, engineSlider, engineLabel, shieldSlider, shieldLabel)
									
end

-- functions required to communicate between slider and ship settings
function powerShift(value)
	value = value*3.0
	local change = ((value - PLAYER:GetDamageBooster())/3.0) * 100
	local compensate =(-change)/2  
	print("power shift value=" .. value)
	changePower(compensate, change ,compensate)
end
function shieldShift(value)
	value = value*3.0
	local change = ((value - PLAYER:GetShieldBooster())/3.0) * 100
	local compensate =(-change)/2  
	print("shield shift value=" .. value)
	changePower(change, compensate ,compensate)
end
function engineShift(value)
	value = value*3.0
	local change = ((value - PLAYER:GetEngineBooster())/3.0) * 100
	local compensate =(-change)/2  
	print("power engine value=" .. value)
	changePower(compensate, compensate ,change)
end
 	
---Change Power Distribution
--- variables given by the desired increase in percentage
function changePower( shield ,damage ,engine)
	shield = shield*0.03
	damage = damage*0.03
	engine = engine*0.03
	local s = PLAYER:GetShieldBooster()
	print("PLAYER:GetShieldBooster()=" .. s ..'\n')
	local newshield = shield + PLAYER:GetShieldBooster()
	local newdamage = damage + PLAYER:GetDamageBooster()
	local newengine = engine + PLAYER:GetEngineBooster()
	
	
	
	--check boundaries
	newshield ,newdamage , newengine = lowerBoundCheck(shield , damage ,engine ,newshield ,newdamage , newengine)
	newdamage ,newshield , newengine=lowerBoundCheck(damage , shield ,engine ,newdamage ,newshield , newengine)
	newengine ,newdamage , newshield=lowerBoundCheck(engine , damage ,shield ,newengine ,newdamage , newshield)
	if (math.floor( (newshield*33.333) + 0.5 )>99) then
		HUD.newAlert("shields at maximum capacity can not raise them any higher")
		return
	end
	if(math.floor( (newdamage*33.333) + 0.5 )>99) then
		HUD.newAlert("power at maximum capacity can not raise them any higher")
		return
	end
	if(math.floor( (newengine*33.333) + 0.5 )>99) then
		HUD.newAlert("Engines at maximum capacity can not raise them any higher")
		return
	end

	newengine = math.floor(newengine * 1000 + 0.5)/1000
	newshield = math.floor(newshield * 1000 + 0.5)/1000
	newdamage = math.floor(newdamage * 1000 + 0.5)/1000
	PLAYER:SetEngineBooster(newengine)
	PLAYER:SetShieldBooster(newshield)
	PLAYER:SetDamageBooster(newdamage)
	
	if(powerSlider and shieldSlider and engineSlider ~=0) then
		powerSlider:setSliderValue(newdamage/3.0)
		shieldSlider:setSliderValue(newshield/3.0)
		engineSlider:setSliderValue(newengine/3.0)
	end
	print("end of power management function shield=" ..newshield .. "power=" ..newdamage .. "engine=" .. newengine)
	newengine = math.floor(newengine * 333 + 0.5)/10
	newshield = math.floor(newshield * 333 + 0.5)/10
	newdamage = math.floor(newdamage * 333 + 0.5)/10
	HUD.newAlert("Power Distribution: shields " .. (newshield) .. "%% power " .. (newdamage) .. "%% engine " .. (newengine) .. "%%")
end

function lowerBoundCheck(a , b ,c ,newa ,newb , newc) --lower bound check for power management function
	if(newa<0) then
		newa= newa- a
		if not ((a<b) and (a<c)) then
			
			if(b<0) then
				if(newb + a > 0) then
					newb = newb + a
				else
					a = a + newa
					newa=0
					newb =newb +a

				end
			else
				if ( newc + a > 0) then
					newc = newc + a
				else
					a = a + newa
					newa=0
					newc =newc +a
					
				end
			end
		else	
			
			newc= newc + (newa/2) -c
			newb = newb +(newa/2) -b
			newa = 0
			HUD.newAlert("can not go any lower")
		end
	end
	return newa, newb, newc
end

function playerFire()
	if Epiar.ispaused()==1 then return end

	local result = PLAYER:Fire( HUD.getTarget() )

	if result == 0 then -- FireSuccess
	elseif result == 1 then -- FireNoWeapons
		HUD.newAlert("No weapons to fire!")
	elseif result == 2 then -- FireNotReady
	elseif result == 3 then -- FireNoAmmo
		HUD.newAlert("Out of Ammo!")
	else
	end
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
		--local moneyOnBoard = targettedShip:GetTotalCost() + targettedShip:GetCredits() -- old way
		local moneyOnBoard = targettedShip:GetCredits() -- new way

		local targetMass = targettedShip:GetMass()

		-- prob. divisor that attempt will succeed. greater player mass boosts this number.
		local succ_max = 15 ^ ( targetMass / PLAYER:GetMass() )
		-- prob. divisor that ship will destruct. greater player mass diminishes this number.
		local destruct_max = 5 ^ ( PLAYER:GetMass() / targetMass )

		local captureProbPct = (1 / succ_max) * 100

		boardingDialog = UI.newWindow(100, 100, 450, 190, "Boarding Ship")
		boardingDialog:add( UI.newLabel(50, 30,  string.format("You have boarded the ship." ) ) )
		boardingDialog:add( UI.newLabel(50, 60,  string.format("  Class: %s", targettedShip:GetModelName() ) ) )
		boardingDialog:add( UI.newLabel(50, 90,  string.format("  Credits on board: %d", moneyOnBoard ) ) )
		boardingDialog:add( UI.newLabel(50, 120, string.format("  Capture probability: %.0f%%", captureProbPct ) ) )
		boardingDialog:add( UI.newButton(50, 150, 100, 30, "Steal their credits", string.format("doBoarding(%d)", moneyOnBoard ) ) )
		boardingDialog:add( UI.newButton(150, 150, 150, 30, "Attempt to capture vessel", string.format("doCapture(%f, %f)", succ_max, destruct_max) ) )
		boardingDialog:add( UI.newButton(300, 150, 100, 30, "End boarding", string.format("endBoarding()") ) )

	else
		HUD.newAlert("Cannot board target -- too far away")
	end
end

---Hail wrapper for any type of sprite
function hailSprite()
	local targettedSprite = Epiar.getSprite( HUD.getTarget() )
	local spritetype = targettedSprite:GetType()

	if spritetype == 0x01 then
		hailPlanet()
	elseif spritetype == 0x08 then
		hailShip()
	else
		HUD.newAlert("No reply.")
	end
end

function hailPlanet()
	if hailDialog ~= nil then return end -- Abort if the hail dialog is already open

	local targettedPlanet = Epiar.getSprite( HUD.getTarget() )

	if targettedPlanet == nil then
		HUD.newAlert("Cannot hail - no target.")
		return
	end

	HUD.newAlert("Hailing planet...")
	Epiar.pause()

	-- show the dialog
	hailDialog = UI.newWindow(100, 100, 400, 150, "Communication channel")
	hailReplyLabel = UI.newLabel(50, 50, "")

	hailDialog:add( UI.newLabel(50, 30, string.format("Opened a channel to %s:", targettedPlanet:GetName() ) ) )
	hailDialog:add( hailReplyLabel ) 

	hailDialog:add( UI.newButton(50, 100, 100, 30, "Greetings", "doHailGreet()") )
	hailDialog:add( UI.newButton(150, 100, 100, 30, "Rude comment", "doHailInsult()" ) )
	hailDialog:add( UI.newButton(250, 100, 100, 30, "Close channel", "doHailEnd()" ) )

end

---Hail target ship
function hailShip()
	if hailDialog ~= nil then return end -- Abort if the hail dialog is already open

	local targettedShip = Epiar.getSprite( HUD.getTarget() )

	if targettedShip == nil then
		HUD.newAlert("Cannot hail - no target.")
		return
	end

	-- for now, let's only hail non-disabled ships

	didBFM = false

	if (targettedShip:IsDisabled() ~= 1) then
		HUD.newAlert("Hailing ship...")
		Epiar.pause()

		-- show the dialog
		hailDialog = UI.newWindow(100, 100, 400, 150, "Communication channel")
		hailReplyLabel = UI.newLabel(50, 50, "")

		hailDialog:add( UI.newLabel(50, 30, string.format("Opened a channel to the %s:", targettedShip:GetModelName() ) ) )
		hailDialog:add( hailReplyLabel ) 

		hailDialog:add( UI.newButton(50, 100, 100, 30, "Greetings", "doHailGreet()") )
		hailDialog:add( UI.newButton(150, 100, 100, 30, "Beg for mercy", "doHailBFM()" ) )
		hailDialog:add( UI.newButton(250, 100, 100, 30, "Close channel", "doHailEnd()" ) )

	else
		HUD.newAlert("No reply.")
	end
end

function doHailGreet()
	if hailDialog == nil then return end
	local targettedSprite = Epiar.getSprite( HUD.getTarget() )
	local spritetype = targettedSprite:GetType()

	if spritetype == 0x01 then
		if targettedSprite:GetForbidden() == 1 then
			hailReplyLabel.setLabel(hailReplyLabel, string.format("You are not welcome on %s.", targettedSprite:GetName() ) )
		else
			hailReplyLabel.setLabel(hailReplyLabel, string.format("Greetings from %s.", targettedSprite:GetName() ) )
		end
	elseif spritetype == 0x08 then
		hailReplyLabel.setLabel(hailReplyLabel,"Hello there.")
	else
		-- should not happen
	end
end

function doHailInsult()
	if hailDialog == nil then return end
	local targettedPlanet = Epiar.getSprite( HUD.getTarget() )

	if targettedPlanet:GetForbidden() == 1 then
		hailReplyLabel.setLabel(hailReplyLabel,string.format("Stop wasting our time, %s.",PLAYER:GetName()) )
		return
	end

	-- should make this query the planet data for an appropriate response / attitude toward the player
	local r = getRand( os.time() + targettedPlanet:GetID(), 10 )

	if r == 1 then
		hailReplyLabel.setLabel(hailReplyLabel,string.format("Outrageous! You are now banned from %s.",targettedPlanet:GetName()) )
		planet:SetForbidden(1)
	elseif r == 2 then
		hailReplyLabel.setLabel(hailReplyLabel,string.format("Here's 100 credits - now please leave us alone.",targettedPlanet:GetName()) )
		addcredits( 100 )
	else 
		hailReplyLabel.setLabel(hailReplyLabel,"We are saddened by your insults.")
	end
end
	
function doHailBFM()
	if hailDialog == nil then return end
	local targettedShip = Epiar.getSprite( HUD.getTarget() )

	if didBFM then
		HUD.newAlert(string.format("The %s closed the channel.", targettedShip:GetModelName() ) )
		doHailEnd()
	end

	local r = getRand( os.time() + targettedShip:GetID(), 25 )

	if ( r == 1 ) then
		hailReplyLabel.setLabel(hailReplyLabel,"Very well; I'm feeling gracious at the moment.")
		AIData[targettedShip:GetID()].target = -1
		-- 'friendly' means will never arbitrary select player as a target unless provoked
		targettedShip:SetFriendly(1)
	else
		hailReplyLabel.setLabel(hailReplyLabel,"I don't think so.")
		didBFM = 1
	end
end
	
function doHailEnd()
	if hailDialog == nil then return end

	Epiar.unpause()
	hailDialog:close()
	hailDialog = nil
end

function getRand(seed, top)
	math.randomseed( seed )

	for s = 1,10 do
		math.random()
	end

	local r = math.random(top)

	return r
end


	

--- Callback for the UI button in boarding ship dialog (see above)
function doBoarding( reward )
	if boardingDialog == nil then return end --- this should never happen

	addcredits( reward )

	Epiar.getSprite( HUD.getTarget() ) : SetCredits( 0 )

	Epiar.unpause()
	boardingDialog:close()
	boardingDialog = nil
end

function endBoarding()
	Epiar.unpause()
	boardingDialog:close()
	boardingDialog = nil
end

function doCapture(succ_max, destruct_max)
	local targettedShip = Epiar.getSprite( HUD.getTarget() )

	local r_succ = getRand( os.time() + targettedShip:GetID(), succ_max )
	local r_selfdestruct = getRand( os.time() + targettedShip:GetID(), destruct_max )

	--print ( string.format("smax=%d dmax=%d rsucc=%d rdestruct=%d", succ_max, destruct_max, r_succ, r_selfdestruct) )

	if r_selfdestruct == 1 then
		HUD.newAlert(string.format("Your boarding party set off the %s's self-destruct mechanism.", targettedShip:GetModelName() ) )
		endBoarding()
		targettedShip:SetShieldDamage(10000)
		targettedShip:SetHullDamage(10000)

	elseif r_succ == 1 then -- success!
		HUD.newAlert(string.format("It's your %s now!", targettedShip:GetModelName() ) )

		local oldPlayerModel = PLAYER:GetModelName() 
		--local oldPlayerX, oldPlayerY = PLAYER:GetPosition() 
		local oldPlayerHD = PLAYER:GetHullDamage() 
		local oldPlayerSD = PLAYER:GetShieldDamage() 

		--print (string.format ("opm=%s opp=%f,%f ophd=%d opsd=%d\n", oldPlayerModel, oldPlayerX, oldPlayerY, oldPlayerHD, oldPlayerSD) )

		PLAYER:SetModel( targettedShip:GetModelName() )
		--PLAYER:SetPosition( targettedShip:GetPosition() ) -- would like to swap positions too, but this is not critical
		PLAYER:SetHullDamage( targettedShip:GetHullDamage() )
		PLAYER:SetShieldDamage( targettedShip:GetHullDamage() )
		PLAYER:Repair( 10 )

		targettedShip:SetModel( oldPlayerModel ) 
		--targettedShip:SetPosition( oldPlayerX, oldPlayerY ) 
		targettedShip:SetHullDamage( oldPlayerHD ) 
		targettedShip:SetShieldDamage( oldPlayerSD ) 

		endBoarding()

	else
		HUD.newAlert(string.format("You were not able to gain control of the %s.", targettedShip:GetModelName() ) )
	end

end

--- Try to land
function attemptLanding()
	if landingWin ~= nil then return end

	local x,y = PLAYER:GetPosition()
	local planet = Epiar.nearestPlanet(PLAYER, 4096)
	local px,py = planet:GetPosition()
	local distance = distfrom(px, py, x,y)
	local message = ""

	if HUD.getTarget() ~= planet:GetID() then -- Add this text before the first message.
		message = string.format("This is %s Landing Control. ", planet:GetName())
		
		-- Check if the ship is close enough and moving slowly enough to land on the planet.
		HUD.setTarget(planet:GetID())
		HUD.newAlert(message)
	else
		if planet:GetForbidden() == 1 then
			HUD.newAlert(string.format("%s: %s! You are forbidden from landing here.", planet:GetName(), PLAYER:GetName() ) )
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
radarZoomKeys()

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
	--local weaponsAndAmmo = PLAYER:GetWeapons()
	local weapSlotContents = PLAYER:GetWeaponSlotContents()
	--for weapon,ammo in pairs(weaponsAndAmmo) do
	for name,weap in pairs(weapSlotContents) do
		HUD.newStatus(weap..":",130,0, string.format("playerAmmo('%s')",weap))
	end

	-- Target Bars
	HUD.newStatus("Target (N):",130,1, "HudTargetName()")
	HUD.newStatus("Target (H):",130,1, "HudTargetHull()")
	HUD.newStatus("Target (S):",130,1, "HudTargetShield()")
end

function playerAmmo(weaponName)
	local weaponsAndAmmo = PLAYER:GetWeapons()
	local ammo = "---"
	if weaponsAndAmmo == nil then
		return ammo
	end
	if weaponsAndAmmo[weaponName] ~= nil then
			ammo = string.format("%d",weaponsAndAmmo[weaponName])
	end

	-- with weapon groups, this convention no longer makes sense
	--if weaponName == PLAYER:GetCurrentWeapon() then -- FIXME need to make this also check which SLOT is being used
	--	ammo = ammo .. " ARMED"
	--end
	return ammo
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
	local videoWidth = Video.getWidth()
	local videoHeight = Video.getHeight()

	loadingWin = UI.newWindow( (videoWidth / 2) - (width / 2), (videoHeight / 2) - (height / 2), width, height,"Load a Player" )

	--- Load an old Player
	yoff = 30
	if #players > 0 then
		loadingWin:add( UI.newLabel( 30, yoff, "Load one of your old saved games:" ) )
		yoff = yoff + 30
		for i=1,#players do
			local player = players[i]
			-- TODO: show a preview of the player (curret ship, location, equipment)
			player = string.gsub(player, "([\\\"])", "\\%1")
			loadingWin:add( UI.newButton(width/2-50,yoff,100,30,player,string.format("loadPlayer(\"%s\")",player)))
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
	if FailureWindow ~= nil then return end
	local name = trim( playerNameField:GetText() )
	if name == "" then
		NewFailureWindow("Bad Player Name", "You can't use an empty string for a player name.")
		return
	end
	Epiar.newPlayer(name)
	loadingWin:close()
	playerStart()
	intro()
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
	local y = y+110

	local infoTabs = UI.newTabCont( 15, y, width-30, height-y-30, "Info Tabs" )

	-- The Outfit Tab
	local outfitTab = UI.newTab( "Outfit" )
	y = 10
	outfitTab:add( UI.newLabel(20, y, "Weapons:") )
	local weaponsAndAmmo = PLAYER:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		y = y+20
		outfitTab:add( UI.newLabel(30, y, weapon) )
		outfitTab:add( UI.newLabel(230, y, ammo) )
	end

	y = y+40
	outfitTab:add( UI.newLabel(20, y, "Outfit:") )
	local outfits = PLAYER:GetOutfits()
	for i,outfit in pairs(outfits) do
		y = y+20
		outfitTab:add( UI.newLabel(30, y, outfit) )
	end

	y = y+40
	outfitTab:add( UI.newLabel(20, y, "Cargo:") )
	local cargo = PLAYER:GetCargo()
	for cargoName,cargoAmount in pairs(cargo) do
		y = y+20
		outfitTab:add( UI.newLabel(30, y, cargoName) )
		outfitTab:add( UI.newLabel(230, y, cargoAmount) )
	end

	-- The Missions Tab
	local missionTab = UI.newTab( "Missions" )
	y = 10

	local missions = PLAYER:GetMissions()
	print( missions, #missions)
	if #missions > 0 then
		for key,mission in pairs(missions) do
			missionTab:add( UI.newLabel( 10, y, "["..key.."] "..mission.Name ) )
			missionTab:add( UI.newButton(width-55, y+3, 10, 20, "x", string.format("PLAYER:RejectMission('%s')", mission.Name) ) )
			y = y + 20
		end
	else
		missionTab:add( UI.newLabel( 10, y, "You have no current jobs." ) )
	end

	infoWin:add( infoTabs )
	infoTabs:add( outfitTab, missionTab )

end
