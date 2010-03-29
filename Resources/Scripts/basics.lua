--
-- Basic Scenario

--- Trader AI
function Trader(id,x,y,angle,speed,vector,state)
	local cur_ship = Epiar.getSprite(id)
	local newstate = state
	local target,px,py,dist
	-- Pre Action setup --
	if AIPlans[id]==nil then
		state="reset" -- reset
	else
		px = AIPlans[id].x
		py = AIPlans[id].y
		target = AIPlans[id].planet
		dist = distfrom(px,py,x,y)
	end
	-- Act on the State --
	if state=="Docking" then
		-- Stop on this planet
		if speed > 0.5 then
			cur_ship:Rotate( - cur_ship:directionTowards( cur_ship:GetMomentumAngle() ) )
			if dist>100 and math.abs(180 - math.abs(cur_ship:GetMomentumAngle() - cur_ship:GetAngle())) <= 10 then
				cur_ship:Accelerate()
			end
		end
		-- If we drift away, then find a new planet
		if dist > 800 then
			newstate="Find New"
		end
	elseif state=="Travelling" then
		-- Get to the planet
		cur_ship:Rotate( cur_ship:directionTowards(px,py) )
		cur_ship:Accelerate()
		if dist < 800 then
			newstate = "Docking"
		end
	else
		-- Choose a planet
		local planetNames = Epiar.planetNames()
		local target = Planet.Get(planetNames[ math.random(#planetNames) ])
		local px,py = target:GetPosition()
		AIPlans[id] = {x=px,y=py,planet=target}
		newstate="Travelling"
	end
	return newstate
end

--- Hunter AI
function Hunter(id,x,y,angle,speed,vector,state)
	local cur_ship = Epiar.getSprite(id)
	local newstate = state
	local target,tx,ty,dist
	-- Pre Action setup --
	if AIPlans[id]==nil then
		state="reset" -- reset
	else
		target = Epiar.getSprite( AIPlans[id].target )
		if target~=nil then
			tx,ty = target:GetPosition()
			AIPlans[id] = {x=tx,y=ty,target=target:GetID()}
			dist = distfrom(tx,ty,x,y)
		else
			-- The Target is dead
			state = "Finding New Target"
		end
	end
	-- Act on the State --
	if state=="Killing" then
		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )
		cur_ship:Fire()
		if dist>100 then
			cur_ship:Accelerate()
		end
		if dist>300 then
			newstate="Hunting"
		end
		if target==nil or target:GetHull()==0 then
			HUD.newAlert(string.format("%s #%d:Victory is Mine!",cur_ship:GetModelName(),id))
			newstate="Killed It!"
		end
	elseif state=="Hunting" then
		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )
		cur_ship:Accelerate()
		if dist<300 then
			newstate="Killing"
		end
	elseif state=="Wandering" then
		-- Head to the planet
		cur_ship:Rotate( cur_ship:directionTowards(tx,ty) )
		cur_ship:Accelerate()
		-- If anything gets within 1000 clicks of me, hunt it
		local ship= Epiar.nearestShip(cur_ship,1000)
		if ship~=nil then
			tx,ty = ship:GetPosition()
			AIPlans[id] = {x=tx,y=ty,target=ship:GetID()}
			newstate="Hunting"
		end
	else
		newstate="Wandering"
		-- Head to a planet.
		-- That's where all of the Victims usually are
		local p = Epiar.nearestPlanet(cur_ship,1000)
		if p==nil then
			-- Choose a random planet
			local planetNames = Epiar.planetNames()
			p = Planet.Get(planetNames[ math.random(#planetNames) ])
		end
		tx,ty = p:GetPosition()
		AIPlans[id] = {x=tx,y=ty,target=p:GetID()}
	end
	return newstate
end


