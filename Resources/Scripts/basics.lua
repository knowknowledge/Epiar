--------------------------------------------------------------------------------
-- Basic Scenario

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

function aimCenter(cur_ship,timeleft)
	-- direction towards the center or the universe
	if timeleft%3 ==0 then
		cur_ship:Rotate( cur_ship:directionTowards(0,0) )
	end
	cur_ship:Accelerate()
end

function chaseClosure(targetShip)
	function plan(cur_ship,timeleft)
		myx,myy = cur_ship:GetPosition()
		x,y = targetShip:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(x, y) )
		cur_ship:Accelerate()
		if distfrom(myx,myy,x,y)<200 then
			cur_ship:Fire()
		end
	end
	return plan 
end

function fleeClosure(targetShip)
	function plan(cur_ship,timeleft)
		x,y = targetShip:GetPosition()
		cur_ship:Rotate( -cur_ship:directionTowards(x, y) )
		cur_ship:Accelerate()
	end
	return plan 
end

function fleePoint(x,y)
	function flee(cur_ship,timeleft)
		cur_ship:Rotate( -cur_ship:directionTowards(x, y) )
		cur_ship:Accelerate()
	end
	return plan
end

function nearestPlanet(cur_ship,timeleft)
	planet = Epiar.nearestPlanet(cur_ship,4096)
	if planet ~= nil then
		myx,myy = cur_ship:GetPosition()
		x,y = planet:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(x, y) )
		if distfrom(myx,myy,x,y)>200 then
			cur_ship:Accelerate()
		end
	else
		cur_ship:Accelerate()
	end
end

function stop(cur_ship,timeleft)
end

function landOnNearestPlanet(cur_ship,timeleft)
	planet = Epiar.nearestPlanet(cur_ship,4096)
	if planet ~= nil then
		myx,myy = cur_ship:GetPosition()
		x,y = planet:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(x, y) )
		if distfrom(myx,myy,x,y)<100 then
			cur_ship:Remove()
		end
	end
	cur_ship:Accelerate()
end

function nearestShip(cur_ship,timeleft)
	ship = Epiar.nearestShip(cur_ship,4096)
	if ship then
		myx,myy = cur_ship:GetPosition()
		x,y = ship:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(x, y) )
		if distfrom(myx,myy,x,y)<200 then
			cur_ship:Fire()
		end
	else
		cur_ship:Rotate( cur_ship:directionTowards(0, 0) )
	end
	cur_ship:Accelerate()
end

chasePlayer = chaseClosure( PLAYER )
fleePlayer = fleeClosure( PLAYER )

-- Zig, Then Zag
function zigzag(cur_ship,timeleft)
	-- Change direction rapidly
	if timeleft % 10 <=3 then
		cur_ship:Rotate(1)
	elseif timeleft % 10 >=7 then
		cur_ship:Rotate(-1)
	end
	if timeleft % 10 == 5 then
		cur_ship:ChangeWeapon()
	end
	cur_ship:Accelerate()
end

function moreTraffic(tickcycle)
	ticks = tickcycle
	function traffic()
		ticks = ticks -1
		if ticks == 0 then
			planetTraffic()
			ticks = tickcycle
		end
	end
	return traffic
end


-- Closure to create a bounding box function to keep all ships near
function boundingClosure(distance, ticks)
	function boundingBox()
		ships = Epiar.ships()
		-- Move Non-Player ships
		for s =1, #ships do
			cur_ship = ships[s]
			n =  cur_ship:GetID()
			if AIPlans[n].time < 3 then
				x,y = cur_ship:GetPosition()
				dist = distfrom(x,y,0,0)
				if dist > distance then
					AIPlans[n] = {plan=aimCenter,time=ticks}
				end
			end
		end
	end
	return boundingBox
end

-- Register the Basics
registerPlan(nearestShip)
registerPlan(stop)
registerPlan(aimCenter)
registerPlan(zigzag)
registerPlan(chasePlayer)
registerPlan(fleePlayer)
registerPlan(nearestPlanet)
--registerPostStep(boundingClosure(2000,300))
registerPostStep(moreTraffic(1000))

