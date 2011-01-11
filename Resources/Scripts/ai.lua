--
-- Basic AI
--[[

An AI StateMachine must have the form:
StateMachine = {
	State = function(id,x,y,angle,speed,vector) ... end,
	...
}

All StateMachines must have a "default" state.  This is their initial state.
States transition by returning a string of the new State's name.
States that do not return new state names will stay in the same state.

--]]

AIData = {}

function FindADestination(id,x,y,angle,speed,vector)
	-- Choose a planet
	local cur_ship = Epiar.getSprite(id)

	if AIData[id].destinationName ~= nil and AIData[id].alwaysGateTravel == true then
		return GateTraveler.ComputingRoute(id,x,y,angle,speed,vector)
	end

	local planetNames = Epiar.planetNames()
	local destination = Planet.Get(planetNames[ math.random(#planetNames) ])
	AIData[id].destination = destination:GetID()
	AIData[id].destinationName = destination:GetName()
	return "Travelling"
end

function okayTarget(cur_ship, ship)
	-- if friendly (merciful) mode is on and the nearest target is the player, forbid this target
	if cur_ship:GetFriendly() == 1 and ship:GetID() == 60 then
		return false
	end

	if AIData[ship:GetID()] == nil then return true end

	-- If the ship in question is accompanying either this ship or whichever one we are
	-- accompanying, don't attack it.
	if AIData[ship:GetID()].accompany == cur_ship:GetID() or
	   AIData[ship:GetID()].accompany == AIData[cur_ship:GetID()].accompany then
		return false
	end

	return true
end

function setAccompany(id, accid)
	if AIData[id] == nil then
		AIData[id] = { }
	end
	if Epiar.getSprite(accid) ~= nil then
		AIData[id].accompany = accid
	else
		AIData[id].accompany = -1
	end
end

function setHuntHostile(id, tid)
	if AIData[id] == nil then
		AIData[id] = { }
	end
	AIData[id].target = tid
	AIData[id].hostile = 1
	AIData[id].foundTarget = 0
end

-- Gate Traveler AI to be used by others
GateTraveler = {
	default = function(id,x,y,angle,speed,vector)
		print "WARNING: GateTraveler default state was invoked. This should not happen."
		local cur_ship = Epiar.getSprite(id)
		cur_ship:Explode()
		return "default"
	end,
	ComputingRoute = function(id,x,y,angle,speed,vector)
		if AIData[id].Autopilot == nil then
			AIData[id].Autopilot = APInit( "AI", id )
		end
		if AIData[id].Autopilot.spcr == nil then
			AIData[id].Autopilot:compute( AIData[id].destinationName )
		else
			local finished = AIData[id].Autopilot.spcrTick()
			if finished then 
				return "GateTravelling"
			else
				return "ComputingRoute"
			end
		end
	end,
	GateTravelling = function(id,x,y,angle,speed,vector)
		local cur_ship = Epiar.getSprite(id)
		if AIData[id].Autopilot == nil then
			return "default"
		end
		if AIData[id].Autopilot.AllowAccel ~= false then
			cur_ship:Accelerate()
		end

		local enroute = AIData[id].Autopilot:autoAngle()
		if enroute then return "GateTravelling" end

		AIData[id].destination = -1
		AIData[id].destinationName = nil
		return "Travelling" -- Once the autopilot finishes up, revert back to standard Travelling state
	end
}

--- Hunter AI
Hunter = {
	default = function(id,x,y,angle,speed,vector)
		if AIData[id] == nil then
			AIData[id] = { }
		end
		if AIData[id].foundTarget == 1 then
			AIData[id].hostile = 0
			AIData[id].foundTarget = 0
		end
		return "New_Planet"
	end,
	New_Planet = FindADestination,

	Hunting = function(id,x,y,angle,speed,vector)
		-- Approach the target
		local cur_ship = Epiar.getSprite(id)
		local tx,ty,dist
		local target = Epiar.getSprite( AIData[id].target )
		if target~=nil then
			tx,ty = target:GetPosition()
			dist = distfrom(tx,ty,x,y)
		else
			AIData[id].hostile = 0
			return "default"
		end

		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )

		if math.abs(cur_ship:directionTowards(tx,ty)) == 0 then
			cur_ship:Accelerate()
		end

		if dist<400 then
			return "Killing"
		end
		if dist>1000 and AIData[id].hostile == 0 then
			return "default"
		end

		return "Hunting"
	end,
	Killing = function(id,x,y,angle,speed,vector)
		-- Attack the target
		local cur_ship = Epiar.getSprite(id)
		local tx,ty,dist
		local target = Epiar.getSprite( AIData[id].target )
		if target==nil or target:GetHull()==0 then
			HUD.newAlert(string.format("%s #%d:Victory is Mine!",cur_ship:GetModelName(),id))
			return "default"
		else
			tx,ty = target:GetPosition()
			dist = distfrom(tx,ty,x,y)
		end

		if AIData[id].hostile == 1 and AIData[id].foundTarget == 0 then
			AIData[id].foundTarget = 1
			local machine, state = cur_ship:GetState()
			--HUD.newAlert(string.format("%s %s: Die, %s!", machine, cur_ship:GetModelName(), target:GetName()))
		end

		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )
		local fireResult = cur_ship:Fire( AIData[id].target )

		-- if this firing group isn't doing anything, switch
		if fireResult == 3 or fireResult == 4 then -- FireNoAmmo or FireEmptyGroup
			cur_ship:ChangeWeapon()
		end
		
		if dist>200 then
			if cur_ship:directionTowards(tx,ty) == 0 then
				cur_ship:Accelerate()
			end
		end
		if dist>300 then
			return "Hunting"
		end
	end,

	ComputingRoute = GateTraveler.ComputingRoute,
	GateTravelling = GateTraveler.GateTravelling,
	Travelling = function(id,x,y,angle,speed,vector)
		-- Find a new target
		local cur_ship = Epiar.getSprite(id)
		local closeShip= Epiar.nearestShip(cur_ship,1000)
		local targetShip= nil
		if AIData[id].target ~= nil then targetShip = Epiar.getSprite(AIData[id].target) end

		if targetShip~=nil and okayTarget(cur_ship, targetShip) and AIData[id].hostile == 1 then
			return "Hunting"
		elseif closeShip~=nil and okayTarget(cur_ship, closeShip) then
			AIData[id].hostile = 0
			AIData[id].target = closeShip:GetID()
			return "Hunting"
		end


		--print (string.format ("%s %s not hunting anything target %d\n", cur_ship:GetState(), AIData[id].target))

		AIData[id].hostile = 0

		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if distfrom(px,py,x,y) < 800 then
			return "New_Planet"
		end
	end,
}

--- Trader AI
Trader = {
	Hunting = Hunter.Hunting,
	Killing = Hunter.Killing,
	Docking = function(id,x,y,angle,speed,vector)
		if AIData[id].hostile == 1 then return "Hunting" end
		-- Stop on this planet
		local cur_ship = Epiar.getSprite(id)
		local p = Epiar.getSprite( AIData[id].destination )

		local px,py = p:GetPosition()
		local dist = distfrom(px,py,x,y)
		if speed > 0.5 then
			cur_ship:Rotate( - cur_ship:directionTowards( cur_ship:GetMomentumAngle() ) )
			if dist>100 and math.abs(180 - math.abs(cur_ship:GetMomentumAngle() - cur_ship:GetAngle())) <= 10 then
				cur_ship:Accelerate()
			end
		end
		-- If we drift away, then find a new planet
		if dist > 800 then
			return "New_Planet"
		end
	end,
	ComputingRoute = GateTraveler.ComputingRoute,
	GateTravelling = GateTraveler.GateTravelling,
	Travelling = function(id,x,y,angle,speed,vector)
		if AIData[id].hostile == 1 then return "Hunting" end
		-- Get to the planet
		local cur_ship = Epiar.getSprite(id)

		local p = Epiar.getSprite( AIData[id].destination )
		if p == nil then return "New_Planet" end
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if distfrom(px,py,x,y) < 800 then
			return "New_Planet"
		end
	end,
	New_Planet = FindADestination,
	default = function(id,x,y,angle,speed,vector,state)
		if AIData[id] == nil then AIData[id] = { } end
		local cur_ship = Epiar.getSprite(id)

		local traderNames = {	"S.S. Epiar", "S.S. Honorable", "S.S. Marvelous", "S.S. Delight",
					"S.S. Honeycomb", "S.S. Woodpecker", "S.S. Crow", "S.S. Condor",
					"S.S. Windowpane", "S.S. Marketplace", "S.S. Baker", "S.S. Momentous",
					"S.S. Robinson", "S.S. Andersonville", "S.S. Ash", "S.S. Maple",
					"S.S. Mangrove", "S.S. Cheetah", "S.S. Apricot", "S.S Amicable",
					"S.S. Schumacher", "S.S. Bluebird", "S.S. Bluejay", "S.S. Hummingbird",
					"S.S. Nightcap", "S.S. Starsplash", "S.S. Starrunner", "S.S. Starfinder" }
		cur_ship:SetName(traderNames[math.random(#traderNames)])

		return "New_Planet"
	end,
}

Patrol = {
	default = function(id,x,y,angle,speed,vector)
		local cur_ship = Epiar.getSprite(id)
		AIData[id] = {}
		destination = Epiar.nearestPlanet(cur_ship,4096)
		if destination==nil then
			return "New_Planet"
		end
	  	AIData[id].destination = destination:GetID()
		return "Travelling"
	end,
	New_Planet = FindADestination,
	Hunting = Hunter.Hunting,
	Killing = Hunter.Killing,
	ComputingRoute = GateTraveler.ComputingRoute,
	GateTravelling = GateTraveler.GateTravelling,
	Travelling = function(id,x,y,angle,speed,vector)
		if AIData[id].hostile == 1 then return "Hunting" end
		local cur_ship = Epiar.getSprite(id)
		local p = Epiar.getSprite( AIData[id].destination )
		if p == nil then return "default" end
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if distfrom(px,py,x,y) < 1000 then
			return "Orbiting"
		end
	end,
	Orbiting = function(id,x,y,angle,speed,vector)
		if AIData[id].hostile == 1 then return "Hunting" end
		local cur_ship = Epiar.getSprite(id)
		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		local dist = distfrom(px,py,x,y)
		cur_ship:Accelerate()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) +90)
		if dist > 1500 then
			return "TooFar"
		end
		if dist < 500 then
			return "TooClose"
		end
		local ship= Epiar.nearestShip(cur_ship,1000)
		if ship~=nil and okayTarget(cur_ship, ship) then
			local machine, state = ship:GetState()
			if machine=="Hunter" then
				AIData[id].target = ship:GetID()
				return "Hunting"
			end
		end
	end,
	TooClose = function(id,x,y,angle,speed,vector)
		if AIData[id].hostile == 1 then return "Hunting" end
		local cur_ship = Epiar.getSprite(id)
		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		cur_ship:Rotate( - cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if distfrom(px,py,x,y) > 800 then
			return "Orbiting"
		end
	end,
	TooFar = function(id,x,y,angle,speed,vector)
		if AIData[id].hostile == 1 then return "Hunting" end
		local cur_ship = Epiar.getSprite(id)
		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if distfrom(px,py,x,y) < 1300 then
			return "Orbiting"
		end
	end,
}


Bully = {
	default = Patrol.default,
	New_Planet = FindADestination,
	ComputingRoute = GateTraveler.ComputingRoute,
	GateTravelling = GateTraveler.GateTravelling,
	Travelling = Patrol.Travelling,
	TooClose = Patrol.TooClose,
	TooFar = Patrol.TooFar,
	Hunting = Hunter.Hunting,
	Killing = Hunter.Killing,

	Orbiting = function(id,x,y,angle,speed,vector)
		if AIData[id].hostile == 1 then return "Hunting" end
		local cur_ship = Epiar.getSprite(id)
		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		local dist = distfrom(px,py,x,y)
		local ship= Epiar.nearestShip(cur_ship,900)
		if (ship~=nil) and (ship:GetID() ~= id) and (ship:GetHull() <= 0.9) and (okayTarget(cur_ship, ship)) then
			AIData[id].target = ship:GetID()
			return "Hunting"
		end

		if dist > 1500 then
			return "TooFar"
		end
		if dist < 500 then
			return "TooClose"
		end
		cur_ship:Rotate( cur_ship:directionTowards(px,py) +90)
		cur_ship:Accelerate()
	end,
}

Pirate = Hunter

Escort = {
	default = function(id,x,y,angle,speed,vector)
		if AIData[id] == nil then
			AIData[id] = { }
			AIData[id].accompany = -1
		end
		AIData[id].target = -1

		-- Create some variation in how escort pilots behave
		local mass = Epiar.getSprite(id):GetMass()
		AIData[id].farThreshold = 225 * mass + math.random(50)
		AIData[id].nearThreshold = 100 * mass + math.random(40)

		local myFleet = Fleets:getShipFleet(id)
		if myFleet ~= nil then setAccompany(id, myFleet:getLeader() ) end

		if AIData[id].accompany >= 0 then return "Accompanying" end
		return "New_Planet"
	end,
	ComputingRoute = GateTraveler.ComputingRoute,
	GateTravelling = GateTraveler.GateTravelling,
	Travelling = function(id,x,y,angle,speed,vector)
		if AIData[id].accompany > -1 then return "Accompanying" end
		return Patrol.Travelling(id,x,y,angle,speed,vector)
	end,
	New_Planet = FindADestination,
	Orbiting = function(id,x,y,angle,speed,vector)
		if AIData[id].accompany > -1 then return "Accompanying" end
		return Patrol.Orbiting(id,x,y,angle,speed,vector)
	end,
	TooClose = function(id,x,y,angle,speed,vector)
		if AIData[id].accompany > -1 then return "Accompanying" end
		return Patrol.TooClose(id,x,y,angle,speed,vector)
	end,
	TooFar = function(id,x,y,angle,speed,vector)
		if AIData[id].accompany > -1 then return "Accompanying" end
		return Patrol.TooFar(id,x,y,angle,speed,vector)
	end,
	Hunting = function(id,x,y,angle,speed,vector)
		local myFleet = Fleets:getShipFleet(id)
		if myFleet ~= nil then
			local prox = myFleet:fleetmateProx(id)
			if prox ~= nil and prox < 45 then return "NewPattern" end
		end
		return Hunter.Hunting(id,x,y,angle,speed,vector)
	end,
	Killing = function(id,x,y,angle,speed,vector)
		local myFleet = Fleets:getShipFleet(id)
		if myFleet ~= nil then
			local prox = myFleet:fleetmateProx(id)
			if prox ~= nil and prox < 45 then return "NewPattern" end
		end
		return Hunter.Killing(id,x,y,angle,speed,vector)
	end,
	NewPattern = function(id,x,y,angle,speed,vector)
		local cur_ship = Epiar.getSprite(id)
		local momentumAngle = cur_ship:GetMomentumAngle()
		if AIData[id].correctAgainst == nil then
			AIData[id].correctAgainst = momentumAngle
			AIData[id].correctOffset = math.random(-90,90)
		end
		cur_ship:Rotate( cur_ship:directionTowards( AIData[id].correctAgainst + AIData[id].correctOffset ) )
		cur_ship:Accelerate()
		if cur_ship:directionTowards( AIData[id].correctAgainst + AIData[id].correctOffset ) == 0 then
			AIData[id].correctAgainst = nil
			AIData[id].correctOffset = nil
			return "Hunting"
		end
	end,
	HoldingPosition = function(id,x,y,angle,speed,vector)
		local ns = AIData[id].nextState
		if ns ~= nil then
			AIData[id].nextState = nil
			return ns
		end

		local cur_ship = Epiar.getSprite(id)
		local inverseMomentumOffset = - cur_ship:directionTowards( cur_ship:GetMomentumAngle() )
		cur_ship:Rotate( inverseMomentumOffset )
		if math.abs( cur_ship:directionTowards( cur_ship:GetMomentumAngle() ) ) >= 176 and speed > 0.1 then
			cur_ship:Accelerate()
		end
		return "HoldingPosition"
	end,
	Accompanying = function(id,x,y,angle,speed,vector)
		local cur_ship = Epiar.getSprite(id)
		local acc = AIData[id].accompany
		local accompanySprite

		if acc > -1 then
			accompanySprite = Epiar.getSprite(AIData[id].accompany)
			if AIData[id].hostile == 1 then return "Hunting" end
			local ns = AIData[id].nextState
			if ns ~= nil then
				AIData[id].nextState = nil
				return ns
			end
		else
			if AIData[id].destination ~= nil and AIData[id].destination > -1 then
				return "Travelling"
			else
				return "New_Planet"
			end
		end

		local ax = 0
		local ay = 0
		local distance

		if accompanySprite~=nil and accompanySprite:GetHull() > 0 then
			ax,ay = accompanySprite:GetPosition()

			local aitype, aitask = accompanySprite:GetState()
			if aitask == "Hunting" or aitask == "Killing" then
				AIData[id].target = AIData[AIData[id].accompany].target
				return "Hunting"
			end
		else
			AIData[id].accompany = -1
			AIData[id].hostile = -1
			return "default"
		end

		distance = distfrom(ax,ay,x,y)

		local accelDir = cur_ship:directionTowards(ax,ay)
		local inverseMomentumDir = - cur_ship:directionTowards( cur_ship:GetMomentumAngle() )

		if distance > AIData[id].farThreshold then
			cur_ship:Rotate( accelDir )
			if accelDir == 0 then
				cur_ship:Accelerate()
			end
		else
			if distance > AIData[id].nearThreshold then
				cur_ship:Rotate( accelDir )
				if distance % (math.sqrt(AIData[id].farThreshold - distance) + 1) < 2 and
				   accelDir == 0 then
					cur_ship:Accelerate()
				end
			else
				cur_ship:Rotate( inverseMomentumDir )
				--if math.abs(inverseMomentumDir) < 35 then
				--	cur_ship:Accelerate()
				--end
			end
		end

		return "Accompanying"
	end,
}

