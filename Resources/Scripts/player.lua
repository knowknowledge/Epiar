-- Use this script for code directly to the Players


--- Keyboard Commands for controlling the Player
-- General commands belong in the defaultCommands table.
playerCommands = {
	-- Each command should be a table
	-- { KEY, TITLE, SCRIPT, KEYMETHOD }
	-- KEYPRESSED triggers every update loop so long as that key is held down
	-- KEYTYPED triggers once every time the key is typed
	{'up', "Accelerate", "PLAYER:Accelerate()",KEYPRESSED},
	{'left', "Turn Left", "PLAYER:Rotate(30)",KEYPRESSED},
	{'right', "Turn Right", "PLAYER:Rotate(-30)",KEYPRESSED},
	{'down', "Reverse", "PLAYER:Rotate(PLAYER:directionTowards(PLAYER:GetMomentumAngle() + 180 ))",KEYPRESSED},
	{'rshift', "Fire Secondary Weapon 1", "playerFire(2)",KEYTYPED},
	{'lshift', "Fire Secondary Weapon 2", "playerFire(2)",KEYTYPED},
	{'tab', "Target Ship", "targetShip()", KEYTYPED},
	{'t', "Target Closest Ship", "targetClosestShip()",KEYTYPED},
	{'i', "Player Info", "playerInformation()", KEYTYPED},
	{'l', "Land on Planet", "attemptLanding()", KEYTYPED},
	{'w', "Watch Target", "Epiar.focusCamera(HUD.getTarget())", KEYDOWN},
	{'w', "Release Watch", "Epiar.focusCamera(PLAYER:GetID())", KEYUP},
	{'space', "Fire Primary Weapon", "playerFire(1)", KEYPRESSED},
	{'b', "Board", "boardShip()", KEYTYPED},
	{'y', "Hail", "hailSprite()", KEYTYPED},
	{'s', "Increase Shields", "changePower(1,-0.5,-0.5)", KEYTYPED},
	{'d', "Increase Power", "changePower(-0.5,1,-0.5)", KEYTYPED},
	{'a', "Increase Engine Power", "changePower(-0.5,-0.5,1)", KEYTYPED},
	{'x', "Decrease Shields", "changePower(-1,0.5,0.5)", KEYTYPED},
	{'c', "Decrease Power", "changePower(0.5,-1,0.5)", KEYTYPED},
	{'z', "Decrease Engine Power", "changePower(0.5,0.5,-1)", KEYTYPED},
	{'P', "Open Power Management Window", "powerManagement()", KEYTYPED},
	{'=', "Equalize Power Management", "equlizePowerManagement()", KEYTYPED},
	{'lalt', "Toggle autopilot", "playerAutopilotToggle()", KEYTYPED},
	{'T', "Configure autopilot", "showAPConfigDialog()", KEYTYPED},
	{'f', "Tell fleet to hunt target", "playerFleetHunt()", KEYTYPED},
	{'g', "Return to formation", "playerFleetFormation()", KEYTYPED},
	{'v', "Hold position", "playerFleetHold()", KEYTYPED}
}

function playerStart()
	PLAYER = Epiar.player()

	-- give the player the standard weapons for this ship model
	for slot,weap in pairs( PLAYER:GetWeaponSlotContents() ) do
		PLAYER:AddToWeaponList(weap)
		-- don't worry about updating the HUD; createHUD() will handle it
	end

	-- Restart Missions
	local missions = PLAYER:GetMissions()
	if #missions > 0 then
		for key,mission in pairs(missions) do
			_G[mission.Type].Create( mission )
		end
	end

	createHUD()
	registerCommands(playerCommands)
end

--- Target ship
function targetShip()
	if Epiar.ispaused()==1 then return end
	local x,y = PLAYER:GetPosition()
	local nearby = Epiar.ships(x,y,4096)
	if #nearby==0 then return end
	
	local nextTarget = 1
	local currentTarget = HUD.getTarget()
	local lastdist = 0

	for s = 1,#nearby-1 do
		if nearby[s]:GetID() == currentTarget then
			nextTarget = s+1
		end
	end
	
	HUD.setTarget(nearby[nextTarget]:GetID()) -- First ID in the list
end

---Target closest ship
function targetClosestShip()
	local nextTarget = Epiar.nearestShip( PLAYER, 4096 )
	if not nextTarget then return end
	HUD.newAlert("Targeting "..nextTarget:GetModelName().." "..nextTarget:GetName())
	HUD.setTarget(nextTarget:GetID())
end

powerSlider, shieldSlider, engineSlider = 0, 0, 0

function powerManagement()
	if powerManagementWindow ~= nil then
		powerManagementWindow:close()
		powerManagementWindow = nil
		return
	end
	local width = 200
	local height = 120
	powerManagementWindow = UI.newWindow(WIDTH/2 - width/2, HEIGHT - height, width, height, "Power Management")
	
	powerSlider = UI.newSlider(20, 40, 80, 16, "Attack Power", PLAYER:GetDamageBooster()/3.0, "powerShift")
	powerLabel = UI.newLabel(105, 35,"Attack Power",0)
	engineSlider = UI.newSlider(20, 65, 80, 16, "Engine", PLAYER:GetEngineBooster()/3.0, "engineShift")
	engineLabel = UI.newLabel(105, 60,"Engine Power",0)
	shieldSlider = UI.newSlider(20, 85, 80, 16, "Shields", PLAYER:GetShieldBooster()/3.0, "shieldShift")
	shieldLabel = UI.newLabel(105, 80,"Shield Strength",0)
	
	powerManagementWindow:add(powerSlider, powerLabel, engineSlider, engineLabel, shieldSlider, shieldLabel)
	powerManagementWindow:addCloseButton()
end

function equlizePowerManagement()
	PLAYER:SetEngineBooster( 1 )
	PLAYER:SetShieldBooster( 1 )
	PLAYER:SetDamageBooster( 1 )
end

-- functions required to communicate between slider and ship settings
function powerShift(value)
	value = value*3.0
	local change = ((value - PLAYER:GetDamageBooster())/3.0) * 100
	local compensate = (-change)/2
	--print("power shift value=" .. value)
	changePower(compensate, change, compensate)
end
function shieldShift(value)
	value = value*3.0
	local change = ((value - PLAYER:GetShieldBooster())/3.0) * 100
	local compensate = (-change)/2
	--print("shield shift value=" .. value)
	changePower(change, compensate, compensate)
end
function engineShift(value)
	value = value*3.0
	local change = ((value - PLAYER:GetEngineBooster())/3.0) * 100
	local compensate = (-change)/2
	--print("power engine value=" .. value)
	changePower(compensate, compensate, change)
end
 	
---Change Power Distribution
--- variables given by the desired increase in percentage
function changePower( shield, damage, engine)
	shield = shield * 0.03
	damage = damage * 0.03
	engine = engine * 0.03
	local s = PLAYER:GetShieldBooster()
	--print("PLAYER:GetShieldBooster()=" .. s ..'\n')
	local newshield = shield + PLAYER:GetShieldBooster()
	local newdamage = damage + PLAYER:GetDamageBooster()
	local newengine = engine + PLAYER:GetEngineBooster()
	
	--check boundaries
	newshield, newdamage, newengine = lowerBoundCheck(shield, damage, engine, newshield, newdamage, newengine)
	newdamage, newshield, newengine = lowerBoundCheck(damage, shield, engine, newdamage, newshield, newengine)
	newengine, newdamage, newshield = lowerBoundCheck(engine, damage, shield, newengine, newdamage, newshield)
	if (math.floor( (newshield*33.333) + 0.5 )>99) then
		HUD.newAlert("Shields at maximum capacity can not raise them any higher")
		return
	end
	if(math.floor( (newdamage*33.333) + 0.5 )>99) then
		HUD.newAlert("Power at maximum capacity can not raise them any higher")
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
	--print("end of power management function shield=" ..newshield .. "power=" ..newdamage .. "engine=" .. newengine)
	--newengine = math.floor(newengine * 333 + 0.5)/10
	--newshield = math.floor(newshield * 333 + 0.5)/10
	--newdamage = math.floor(newdamage * 333 + 0.5)/10
	--HUD.newAlert("Power Distribution: shields " .. (newshield) .. "%% power " .. (newdamage) .. "%% engine " .. (newengine) .. "%%")
end

function lowerBoundCheck(a, b, c, newa, newb, newc) --lower bound check for power management function
	if(newa<0) then
		newa= newa- a
		if not ((a<b) and (a<c)) then
			
			if(b<0) then
				if(newb + a > 0) then
					newb = newb + a
				else
					a = a + newa
					newa = 0
					newb = newb +a

				end
			else
				if ( newc + a > 0) then
					newc = newc + a
				else
					a = a + newa
					newa = 0
					newc = newc +a
					
				end
			end
		else	
			
			newc = newc + (newa/2) -c
			newb = newb + (newa/2) -b
			newa = 0
			HUD.newAlert("can not go any lower")
		end
	end
	return newa, newb, newc
end

function playerFire( primary )
	if Epiar.ispaused() == 1 then return end

	if primary == 1 then
		local result = PLAYER:FirePrimary( HUD.getTarget() )
	else
		local result = PLAYER:FireSecondary( HUD.getTarget() )
	end

	if result == 0 then -- FireSuccess
	elseif result == 1 then -- FireNoWeapons
		HUD.newAlert("No weapons to fire!")
	elseif result == 2 then -- FireNotReady
	elseif result == 3 then -- FireNoAmmo
		HUD.newAlert("Out of Ammo!")
	elseif result == 4 then -- FireEmptyGroup
		HUD.newAlert("No weapons assigned to this firing group")
	else
		-- Some other error
	end
end

---Board closest ship if possible
function boardShip()
	if UI.search("/'Boarding Ship'/") ~= nil then return end -- Abort if the player is already boarding something.

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

		-- prob. divisor that attempt will succeed. greater player mass improves this ratio.
		local succ_max = 4 ^ ( targetMass / PLAYER:GetMass() )
		-- prob. divisor that ship will destruct. greater player mass makes this ratio worse.
		local destruct_max = 5 ^ ( PLAYER:GetMass() / targetMass )

		local captureProbPct = (1 / succ_max) * 100

		local boardingDialog = UI.newWindow(100, 100, 450, 190, "Boarding Ship")
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

	if spritetype == SPRITE_PLANET then
		hailPlanet()
	elseif spritetype == SPRITE_SHIP then
		hailShip()
	else
		HUD.newAlert("No reply.")
	end
end

function hailPlanet()
	if UI.search("/Window'Communication channel'/") ~= nil then return end -- Abort if the hail dialog is already open

	local targettedPlanet = Epiar.getSprite( HUD.getTarget() )

	if targettedPlanet == nil then
		HUD.newAlert("Cannot hail - no target.")
		return
	end

	HUD.newAlert("Hailing planet...")
	Epiar.pause()

	-- show the dialog
	local hailDialog = UI.newWindow(100, 50, 400, 150, "Communication channel")
	hailReplyLabel = UI.newLabel(50, 50, "")

	hailDialog:add( UI.newLabel(50, 30, string.format("Opened a channel to %s:", targettedPlanet:GetName() ) ) )
	hailDialog:add( hailReplyLabel )

	hailDialog:add( UI.newButton(50, 100, 100, 30, "Greetings", "doHailGreet()") )
	hailDialog:add( UI.newButton(150, 100, 100, 30, "Rude comment", "doHailInsult()" ) )
	hailDialog:add( UI.newButton(250, 100, 100, 30, "Close channel", "doHailEnd()" ) )

end

---Hail target ship
function hailShip()
	if UI.search("/Window'Communication channel'/") ~= nil then return end -- Abort if the hail dialog is already open

	if PLAYER:GetName() == nil then
		HUD.newAlert("You can't hail a ship when you're dead!")
		return
	end

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

		local planetNames = Epiar.planetNames()
		local aiDest = planetNames[AIData[targettedShip:GetID()].destination]
		if aiDest == nil then aiDest = "a port" end

		local aiMachDescs = {
			["Trading"]="I'm moving some goods.",
			["Orbiting"]="Oh, I'm just orbiting here.",
			["Hunting"]="If you must know, I'm pursuing a target.",
			["Killing"]="I'm shooting at someone right now!",
			["Docking"]=string.format("I'm docking here at %s.", aiDest),
			["TooClose"]="I'm orbiting, but I'm too close!",
			["TooFar"]="I'm orbiting, but I need to get closer!",
			["Travelling"]=string.format("I'm on my way to %s.", aiDest),
			["Accompanying"]="I'm accompanying another ship.",
		}

		hailResponses = {
			["Greetings"]="Hello there.",
			["What are you up to?"]="Huh, I'm not quite sure what I'm doing right now.",
			["What's the nearest port?"]="Sorry, I'm not sure about that.",
			["Who are you?"]=string.format("This is %s.",targettedShip:GetName()),
			["Do you know who I am?"]=string.format("Well, your identification reads %q.", PLAYER:GetName()),
			["How can I earn money?"]="Try landing on a planet or station and looking in the Employment section.",
			["Your ship looks like junk."]="--",
		}

		if Epiar.nearestPlanet(targettedShip, 4096) ~= nil then
			hailResponses["What's the nearest port?"] = string.format("I guess that would be %s",
				Epiar.nearestPlanet(targettedShip, 4096):GetName())
		end

		local aiState, aiMach = targettedShip:GetState()
		if aiMachDescs[aiMach] ~= nil then
			hailResponses["What are you up to?"] = aiMachDescs[aiMach]
		end

		hailOption1 = "Greetings"
		hailOption2 = "What's the nearest port?"

		-- show the dialog
		hailDialog = UI.newWindow(200, 300, 550, 135, "Communication channel")

		hailDialog:add( UI.newLabel(30, 30, string.format("Opened a channel to the %s:", targettedShip:GetModelName() ) ) )
		hailReplyLabel = UI.newLabel(30, 45, "")

		hailDialog:add( UI.newButton(50, 75, 50, 20, "Say:", "doHailSay(hailOption1)") )
		hailDialog:add( UI.newButton(250, 75, 50, 20, "Say:", "doHailSay(hailOption2)" ) )
		hailDialog:add( UI.newButton(400, 75, 100, 20, "close channel", "doHailEnd()" ) )

		hailOption1Label = UI.newLabel(50, 95, hailOption1)
		hailOption2Label = UI.newLabel(250, 95, hailOption2)
		--hailDialog:add( UI.newButton(250, 100, 100, 30, "Close channel", "doHailEnd()" ) )

		hailDialog:add( hailReplyLabel, hailOption1Label, hailOption2Label )

	else
		HUD.newAlert("No reply.")
	end
end

function doHailSay(said)
	if UI.search("/Window'Communication channel'/") == nil then return end
	local targettedSprite = Epiar.getSprite( HUD.getTarget() )
	local spritetype = targettedSprite:GetType()
	local reply = hailResponses[said]

	if said == "Your ship looks like junk." then
		-- when an AI is in "hostile" mode, it will not abandon its target
		AIData[ HUD.getTarget() ].target = PLAYER:GetID()
		AIData[ HUD.getTarget() ].hostile = 1
		HUD.newAlert( (string.format("%s: We'll see about that!", targettedSprite:GetModelName() ) ) )
		doHailEnd()
		return
	elseif said == "Goodbye" then
		HUD.newAlert (string.format("%s: Goodbye, %s.", targettedSprite:GetModelName(), PLAYER:GetName() ) )
		doHailEnd()
		return
	elseif reply == nil then
		HUD.newAlert (string.format("%s: I think we're done here.", targettedSprite:GetModelName() ) )
		doHailEnd()
		return
	end

	hailReplyLabel.setText(hailReplyLabel, (string.format("%s: %s",targettedSprite:GetModelName(), reply) ) )
	
	hailResponses[said] = nil

	hailOption1 = nil
	hailOption2 = nil

	hailResponses[said] = nil

	for k,v in pairs(hailResponses) do
		if v ~= nil then
			if hailOption1 == nil then hailOption1 = k
			elseif hailOption2 == nil then hailOption2 = k end
		end
	end
	if hailOption1 == nil then hailOption1 = "Goodbye" end
	if hailOption2 == nil then hailOption2 = "Goodbye" end
	
	hailOption1Label.setText(hailOption1Label,hailOption1)
	hailOption2Label.setText(hailOption2Label,hailOption2)

end
	

function doHailGreet()
	if UI.search("/Window'Communication channel'/") == nil then return end
	local targettedSprite = Epiar.getSprite( HUD.getTarget() )
	local spritetype = targettedSprite:GetType()

	if spritetype == SPRITE_PLANET then
		if targettedSprite:GetForbidden() == 1 then
			hailReplyLabel.setText(hailReplyLabel, string.format("You are not welcome on %s.", targettedSprite:GetName() ) )
		else
			hailReplyLabel.setText(hailReplyLabel, string.format("Greetings from %s.", targettedSprite:GetName() ) )
		end
	elseif spritetype == SPRITE_SHIP then
		hailReplyLabel.setText(hailReplyLabel,"Hello there.")
	else
		-- should not happen
	end
end

function doHailInsult()
	if UI.search("/Window'Communication channel'/") == nil then return end
	local targettedPlanet = Epiar.getSprite( HUD.getTarget() )

	if targettedPlanet:GetForbidden() == 1 then
		hailReplyLabel.setText(hailReplyLabel,string.format("Stop wasting our time, %s.",PLAYER:GetName()) )
		return
	end

	-- should make this query the planet data for an appropriate response / attitude toward the player
	local r = math.random( 10 )

	if r == 1 then
		hailReplyLabel.setText(hailReplyLabel,string.format("Outrageous! You are now banned from %s.",targettedPlanet:GetName()) )
		planet:SetForbidden(1) -- FIXME need to store this kind of thing some kind of structure that gets saved when the game is closed
	elseif r == 2 then
		hailReplyLabel.setText(hailReplyLabel,string.format("Here's 100 credits - now please leave us alone.",targettedPlanet:GetName()) )
		addcredits( 100 )
	else
		hailReplyLabel.setText(hailReplyLabel,"We are saddened by your insults.")
	end
end

-- Beg For Mercy
-- WHERE SHOULD THIS BE USED?
function doHailBFM()
	if UI.search("/Window'Communication channel'/") == nil then return end
	local targettedShip = Epiar.getSprite( HUD.getTarget() )

	if didBFM then
		HUD.newAlert(string.format("The %s closed the channel.", targettedShip:GetModelName() ) )
		doHailEnd()
		return
	end

	local r = math.random( 8 )

	if ( r == 1 ) then
		hailReplyLabel.setText(hailReplyLabel,"Very well; I'm feeling gracious at the moment.")
		AIData[targettedShip:GetID()].target = -1
		-- 'merciful' means will never arbitrary select player as a target unless provoked
		targettedShip:SetMerciful(1)
	else
		hailReplyLabel.setText(hailReplyLabel, "I don't think so.")
		didBFM = 1
	end
end
	
function doHailEnd()
	local hailDialog = UI.search("/Window'Communication channel'/")
	if hailDialog == nil then return end

	Epiar.unpause()
	hailDialog:close()
	hailDialog = nil
end

--- Callback for the UI button in boarding ship dialog (see above)
function doBoarding( reward )
	if UI.search("/'Boarding Ship'/") == nil then return end --- this should never happen

	addcredits( reward )

	Epiar.getSprite( HUD.getTarget() ) : SetCredits( 0 )

	Epiar.unpause()
	UI.search("/'Boarding Ship'/"):close()
end

function endBoarding()
	Epiar.unpause()
	local boardingDialog = UI.search("/'Boarding Ship'/")
	if boardingDialog ~= nil then
		boardingDialog:close()
	end
	local successDialog = UI.search("/'Success!'/")
	if successDialog ~= nil then
		successDialog:close()
	end
	asOwn = nil
	asEscort = nil
end

function doCapture(succ_max, destruct_max)
	local targettedShip = Epiar.getSprite( HUD.getTarget() )

	local r_succ = math.random( succ_max )
	local r_selfdestruct = math.random( destruct_max )

	if r_selfdestruct == 1 then
		HUD.newAlert(string.format("Your boarding party set off the %s's self-destruct mechanism.", targettedShip:GetModelName() ) )
		endBoarding()
		targettedShip:SetShieldDamage(10000)
		targettedShip:SetHullDamage(10000)

	elseif r_succ == 1 then -- success!
		HUD.newAlert(string.format("It's your %s now!", targettedShip:GetModelName() ) )

		local partiallyRepair = function(sprite)
			--while sprite:IsDisabled() do
			--while sprite:GetShipHull() < 0.25 do -- 0.15 or lower is disabled
			--	sprite:Repair(40)
			--end
			sprite:Repair(200)
		end

		asOwn = function()
			local oldPlayerModel = PLAYER:GetModelName()
			local oldPlayerHD = PLAYER:GetHullDamage()
			local oldPlayerSD = PLAYER:GetShieldDamage()

			for slot,weap in pairs( PLAYER:GetWeaponSlotContents() ) do
				PLAYER:RemoveFromWeaponList(weap)
				HUD.closeStatus(weap..":");
			end

			PLAYER:SetModel( targettedShip:GetModelName() )
			PLAYER:SetHullDamage( targettedShip:GetHullDamage() )
			PLAYER:SetShieldDamage( targettedShip:GetHullDamage() )
			partiallyRepair(PLAYER)

			targettedShip:SetModel( oldPlayerModel )
			targettedShip:SetHullDamage( oldPlayerHD )
			targettedShip:SetShieldDamage( oldPlayerSD )

			-- SetModel() has already determined the slot contents for us, so use them
			for slot,weap in pairs( PLAYER:GetWeaponSlotContents() ) do
				PLAYER:AddToWeaponList(weap)
				HUD.newStatus(weap..":", 130, UPPER_LEFT, string.format("playerAmmo(%q)",weap))
			end

			endBoarding()
		end

		asEscort = function()
			AIData[ targettedShip:GetID() ].target = -1
			AIData[ targettedShip:GetID() ].hostile = 0
			Fleets:join( PLAYER:GetID(), targettedShip:GetID() )
			targettedShip:SetStateMachine("Escort")
			partiallyRepair(targettedShip)
			PLAYER:AddHiredEscort( targettedShip:GetModelName(), 0, targettedShip:GetID() ) -- this is needed for XML saving
			endBoarding()
		end

		local successDialog = UI.newWindow(100, 100, 450, 190, "Success!")
		successDialog:add( UI.newLabel(50, 30,  string.format("You have successfully captured this vessel." ) ) )
		successDialog:add( UI.newButton(50, 150, 100, 30, "Use as own ship", "asOwn()") )
		successDialog:add( UI.newButton(150, 150, 150, 30, "Use as escort", "asEscort()") )

	else
		HUD.newAlert(string.format("You were not able to gain control of the %s.", targettedShip:GetModelName() ) )
	end

end

--- Try to land
function attemptLanding()
	if UI.search("/Window/Tabs'Store'/") ~= nil then return end

	local x,y = PLAYER:GetPosition()
	local planet = Epiar.nearestPlanet(PLAYER, 4096)
	if planet == nil then return end

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
		if distance > planet:GetSize() then
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
				PLAYER:Land( planet )
			end
		end
	end
end


---Adds to the player's credits
function addcredits( credits )
	playerCredits = PLAYER:GetCredits( )
	PLAYER:SetCredits( credits + playerCredits )
end

--- Teleport to any location via a new gate
function goto(x,y)
	-- If x is a planet name, go to that instead
	for i,planet in pairs( Epiar.planetNames() ) do
		if x == planet then
			x,y = Planet.Get( planet ):GetPosition()
			x = x + about(1000)
			y = y + about(1000)
		end
	end
	local px,py = PLAYER:GetPosition()
	Epiar.NewGatePair(x,y,px,py)
end

--- Register the player functions
function radarZoomKeys()
	for k = 1,9 do
		kn = string.byte(k)
		ks = string.format("%d", 1024*math.pow(2,k-1))
		Epiar.RegisterKey(kn, KEYTYPED, "HUD.setVisibity("..ks..")")
	end
end
radarZoomKeys()

--- Create a HUD
function createHUD()
	-- Location Status Bars
	HUD.newStatus("Coordinate:", 130, UPPER_RIGHT, "string.format('(%d,%d)',PLAYER:GetPosition())")
	HUD.newStatus("Quadrant:", 130, UPPER_RIGHT, "string.format('(%d,%d)',coordinateToQuadrant(PLAYER:GetPosition()))")
	HUD.newStatus("Credits:", 130, UPPER_RIGHT, "string.format('$%d',PLAYER:GetCredits())")

	-- Weapon and Armor Status Bars
	HUD.newStatus("HULL:", 100, UPPER_LEFT, "PLAYER:GetHull()")
	HUD.newStatus("Shield:", 100, UPPER_LEFT, "PLAYER:GetShield()")
	myweapons = {}
	local weapSlotContents = PLAYER:GetWeaponSlotContents()
	--for weapon,ammo in pairs(weaponsAndAmmo) do
	for name,weap in pairs(weapSlotContents) do
		HUD.newStatus(weap..":", 130, UPPER_LEFT, string.format("playerAmmo(%q)",weap))
	end

	-- Power Level
	HUD.newStatus("Power Shield:", 130, UPPER_RIGHT, "(PLAYER:GetShieldBooster() or 0)/3")
	HUD.newStatus("Power Damage:", 130, UPPER_RIGHT, "(PLAYER:GetDamageBooster() or 0)/3")
	HUD.newStatus("Power Engine:", 130, UPPER_RIGHT, "(PLAYER:GetEngineBooster() or 0)/3")

	-- Target Bars
	HUD.newStatus("Target (N):", 130, UPPER_RIGHT, "HudTargetName()")
	HUD.newStatus("Target (H):", 130, UPPER_RIGHT, "HudTargetHull()")
	HUD.newStatus("Target (S):", 130, UPPER_RIGHT, "HudTargetShield()")

end

function playerAmmo(weaponName)
	local weaponsAndAmmo = PLAYER:GetWeaponsAmmo()
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
		if spritetype == SPRITE_PLANET then -- Planet
			return targettedSprite:GetName()
		elseif spritetype == SPRITE_GATE_BOTTOM then -- Gate Bottom
			return "Jump Gate"
		elseif spritetype == SPRITE_WEAPON then -- Projectile
		elseif spritetype == SPRITE_SHIP then -- Ship
			return targettedSprite:GetModelName()
		elseif spritetype == SPRITE_PLAYER then -- Player
			return "You"
		elseif spritetype == SPRITE_GATE_TOP then -- Gate Top
			return "Jump Gate"
		elseif spritetype == SPRITE_EFFECT then -- Effect
		else -- Nothing?
		end
	else -- The Sprite doesn't exist
	end
	return ""
end

function HudTargetHull()
	local targettedSprite = Epiar.getSprite( HUD.getTarget() ) -- Acquire target
	if targettedSprite ~= nil then
		local spritetype = targettedSprite:GetType()
		if (spritetype == SPRITE_SHIP) or (spritetype == SPRITE_PLAYER) then -- Ship or Player
			return targettedSprite:GetHull()
		end
	end
	return 0
end

function HudTargetShield()
	local targettedSprite = Epiar.getSprite( HUD.getTarget() ) -- Acquire target
	if targettedSprite ~= nil then
		local spritetype = targettedSprite:GetType()
		if (spritetype == SPRITE_SHIP) or (spritetype == SPRITE_PLAYER) then -- Ship or Player
			return targettedSprite:GetShield()
		end
	end
	return 0
end

function playerInformation()
	local infoWin = UI.search("/'Player Info'/")
	if infoWin~=nil then
		local descriptionWindow = UI.search("/Window'Mission Description'/")
		if descriptionWindow ~= nil then
			descriptionWindow:close()
			descriptionWindow = nil
		end
		infoWin:close()
		infoWin = nil
		return
	end
	local height = 500
	local width = 300
	infoWin = UI.newWindow( 500,200, width,height, "Player Info")
	local y = 30

	local name = PLAYER:GetName()
	local model = PLAYER:GetModelName()
	local engine = PLAYER:GetEngine()
	local credits = PLAYER:GetCredits()
	infoWin:add( UI.newLabel(20, y, "Name:     ".. name) )
	local y = y+20
	infoWin:add( UI.newLabel(20, y, "Model:    ".. model) )
	local y = y+20
	infoWin:add( UI.newLabel(20, y, "Engine:   ".. engine) )
	local y = y+20
	infoWin:add( UI.newLabel(20, y, "Credits:  ".. credits) )
	local y = y+20
	infoWin:add( UI.newPicture( 20, y, width-40,100, model, 0,0,0,1) )
	local y = y+110

	local infoTabs = UI.newTabContainer( 15, y, width-30, height-y-30, "Info Tabs" )
	infoWin:add( infoTabs )
	infoWin:addCloseButton()

	-- The Outfit Tab
	local outfitTab = UI.newTab( "Outfit" )
	y = 10
	outfitTab:add( UI.newLabel(20, y, "Weapons:") )
	local weaponsAndAmmo = PLAYER:GetWeaponsAmmo()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		y = y+20
		outfitTab:add( UI.newLabel(30, y, weapon) )
		outfitTab:add( UI.newLabel(230, y, ammo) )
	end

	local outfits = PLAYER:GetOutfits()
	if #outfits > 0 then
		y = y+40
		outfitTab:add( UI.newLabel(20, y, "Outfit:") )
		for i,outfit in pairs(outfits) do
			y = y+20
			outfitTab:add( UI.newLabel(30, y, outfit) )
		end
	end

	local cargo = PLAYER:GetCargo()
	if #cargo > 0 then
		y = y+40
		outfitTab:add( UI.newLabel(20, y, "Cargo:") )
		for cargoName,cargoAmount in pairs(cargo) do
			y = y+20
			outfitTab:add( UI.newLabel(30, y, cargoName) )
			outfitTab:add( UI.newLabel(230, y, cargoAmount) )
		end
	end
	infoTabs:add( outfitTab )

	-- The Missions Tab
	local missionTab = UI.newTab( "Missions" )
	y = 10

	local missions = PLAYER:GetMissions()
	if #missions > 0 then
		for key,mission in pairs(missions) do
			missionTab:add( UI.newButton( 6, y, width-40, 30, mission.Name, string.format("ShowMissionDescription(%q,%q)", mission.Name, mission.Description ) ) )
			y = y + 30
		end
	else
		missionTab:add( UI.newLabel( 10, y, "You have no current jobs." ) )
	end
	infoTabs:add( missionTab )

	-- The Favor Tab
	y = 10

	local favorTab = UI.newTab( "Favor" )
	local alliances = Epiar.alliances()
	for i,alliance in ipairs(alliances) do
		local favor = PLAYER:GetFavor( alliance )
		favorTab:add( UI.newLabel( 6, y, string.format("%s: %d", alliance, favor) ) )
		y = y + 30
	end
	infoTabs:add( favorTab )

end

-- Description window for individual mission in mission dialog
function ShowMissionDescription( _missionName, _missionDescription )
	local descriptionWindow = UI.search("/Window'Mission Description'/")
	if descriptionWindow ~= nil then
		descriptionWindow:close()
		descriptionWindow = nil
		--- Don't return, just open the other Mission Window
	end
	local description = UI.newParagraph( 10, 20, 280, 180, _missionDescription )
	local height = description:GetH() + 70
	local rejectButton = UI.newButton( 300-110, height-40, 100, 30, "Abort", string.format("PLAYER:RejectMission(%q); UI.search(\"/Window'Mission Description'/\"):close()", _missionName) )
	local descriptionWindow = UI.newWindow( 100, 100, 300, height, "Mission Description" )
	descriptionWindow:add( description, rejectButton )
	descriptionWindow:addCloseButton()
end

-- This function is called from player.cpp to load escorts from XML.
-- Don't use the PLAYER object in here, as it doesn't exist yet.
function initHiredEscort(playerID, playerX, playerY, escortType, escortPay)
	local name = ("escort " .. escortType)
	local escort = Ship.new(name, playerX - 75 + math.random(150), playerY - 150 + math.random(75), escortType, "Ion Engines", "Escort", "Independent")
	local eid = escort:GetID()
	AIData[eid] = {
		accompany = playerID,
		pay = escortPay,
	}
	Fleets:join(playerID, eid)
	return escort:GetID()
end

