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
	local planetNames = Epiar.planetNames()
	local destination = Planet.Get(planetNames[ math.random(#planetNames) ])
	AIData[id].destination = destination:GetID()
	return "Travelling"
end

function MustDefend(id,attacker)
	local cur_ship = Epiar.getSprite(id)
	if attacker > -1 then
		--print (string.format("lua hunter AI: attacked by id %d! will now hunt %d\n", attacker,attacker))
		AIData[id].target = attacker
		cur_ship:SetAttacker(-1)
		return true
	end
	return false
end

function okayTarget(cur_ship, ship)
	-- if friendly (merciful) mode is on and the nearest target is the player, forbid this target
	if cur_ship:GetFriendly() == 1 and ship:GetID() == 60 then
		return false
	end
	return true
end


--- Trader AI
Trader = {
	Docking = function(id,x,y,angle,speed,vector)
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
	Travelling = function(id,x,y,angle,speed,vector)
		-- Get to the planet
		local cur_ship = Epiar.getSprite(id)

		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if distfrom(px,py,x,y) < 800 then
			return "New_Planet"
		end
	end,
	New_Planet = FindADestination,
	default = function(id,x,y,angle,speed,vector,state)
		AIData[id] = {}
		return "New_Planet"
	end,
}

--- Hunter AI
Hunter = {
	default = function(id,x,y,angle,speed,vector)
		AIData[id] = {}
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
			return "default"
		end
		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )

		if MustDefend(id,cur_ship:GetAttacker()) then return "Hunting" end

		if cur_ship:directionTowards(tx,ty) == 0 then
			cur_ship:Accelerate()
		end


		if dist<400 then
			return "Killing"
		end
		if dist>1000 then
			return "default"
		end
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

		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )
		cur_ship:Fire( AIData[id].target )
		
		if dist>200 then
			if cur_ship:directionTowards(tx,ty) == 0 then
				cur_ship:Accelerate()
			end
		end
		if dist>300 then
			return "Hunting"
		end
	end,

	Travelling = function(id,x,y,angle,speed,vector)
		-- Find a new target
		local cur_ship = Epiar.getSprite(id)
		local ship= Epiar.nearestShip(cur_ship,1000)
		if ship~=nil and okayTarget(cur_ship, ship) then
			tx,ty = ship:GetPosition()
			AIData[id].target = ship:GetID()
			return "Hunting"
		end

		if MustDefend(id,cur_ship:GetAttacker()) then return "Hunting" end

		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
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
	Travelling = function(id,x,y,angle,speed,vector)
		local cur_ship = Epiar.getSprite(id)

		if MustDefend(id,cur_ship:GetAttacker()) then return "Hunting" end

		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if distfrom(px,py,x,y) < 1000 then
			return "Orbiting"
		end
	end,
	Orbiting = function(id,x,y,angle,speed,vector)

		local cur_ship = Epiar.getSprite(id)

		local attacker = cur_ship:GetAttacker()

		if MustDefend(id,cur_ship:GetAttacker()) then return "Hunting" end

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
	Travelling = Patrol.Travelling,
	TooClose = Patrol.TooClose,
	TooFar = Patrol.TooFar,
	Hunting = Hunter.Hunting,
	Killing = Hunter.Killing,

	Orbiting = function(id,x,y,angle,speed,vector)
		local cur_ship = Epiar.getSprite(id)
		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		local dist = distfrom(px,py,x,y)

		if MustDefend(id,cur_ship:GetAttacker()) then return "Hunting" end

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
