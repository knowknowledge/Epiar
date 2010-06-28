--
-- Basic AI
AIData = {}

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
	New_Planet = function(id,x,y,angle,speed,vector)
		-- Choose a planet
		local cur_ship = Epiar.getSprite(id)
		local planetNames = Epiar.planetNames()
		local destination = Planet.Get(planetNames[ math.random(#planetNames) ])
		AIData[id] = {destination= destination:GetID()}
		return "Travelling"
	end,
	default = function(id,x,y,angle,speed,vector,state)
		return "New_Planet"
	end,
}

--- Hunter AI
Hunter = {
	default = function(id,x,y,angle,speed,vector)
		local cur_ship = Epiar.getSprite(id)
		return "New_Planet"
	end,

	Hunting = function(id,x,y,angle,speed,vector)
		-- Approach the target
		local cur_ship = Epiar.getSprite(id)
		local tx,ty,dist
		local target = Epiar.getSprite( AIData[id].target )
		if target~=nil then
			tx,ty = target:GetPosition()
			dist = distfrom(tx,ty,x,y)
		else
			return "Searching"
		end
		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )
		cur_ship:Accelerate()
		if dist<300 then
			return "Killing"
		end
		if dist>1000 then
			return "Searching"
		end
	end,
	Killing = function(id,x,y,angle,speed,vector)
		-- Attack the target
		local cur_ship = Epiar.getSprite(id)
		local tx,ty,dist
		local target = Epiar.getSprite( AIData[id].target )
		if target==nil or target:GetHull()==0 then
			HUD.newAlert(string.format("%s #%d:Victory is Mine!",cur_ship:GetModelName(),id))
			return "Searching"
		else
			tx,ty = target:GetPosition()
			dist = distfrom(tx,ty,x,y)
		end
		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )
		cur_ship:Fire()
		if dist>100 then
			cur_ship:Accelerate()
		end
		if dist>300 then
			return "Hunting"
		end
	end,

	Searching = function(id,x,y,angle,speed,vector)
		-- Find a new target
		local cur_ship = Epiar.getSprite(id)
		local ship= Epiar.nearestShip(cur_ship,1000)
		if ship~=nil then
			tx,ty = ship:GetPosition()
			AIData[id] = {target=ship:GetID()}
			return "Hunting"
		end
		local p = Epiar.getSprite( AIData[id].destination )
		local px,py = p:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
	end,
	New_Planet = function(id,x,y,angle,speed,vector)
		local p = Epiar.nearestPlanet(cur_ship,2000)
		if p==nil then
			-- Choose a random planet
			local planetNames = Epiar.planetNames()
			p = Planet.Get(planetNames[ math.random(#planetNames) ])
		end
		AIData[id] = {destination=p:GetID()}
		return "Searching"
	end,
}


