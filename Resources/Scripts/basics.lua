--------------------------------------------------------------------------------
-- Basic Scenario

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
		x,y = planet:Position()
		cur_ship:Rotate( cur_ship:directionTowards(x, y) )
		if distfrom(myx,myy,x,y)>200 then
			cur_ship:Accelerate()
		end
	else
		cur_ship:Accelerate()
	end
end

function stop(cur_ship,timeleft)
	if cur_ship:GetMomentumSpeed() > 1 then
		cur_ship:Rotate( - cur_ship:directionTowards( cur_ship:GetMomentumAngle() ) )
	end
	if math.abs( cur_ship:GetMomentumAngle() - cur_ship:GetAngle() ) < 10 then
		cur_ship:Accelerate()
	end
end

function landOnNearestPlanet(cur_ship,timeleft)
	planet = Epiar.nearestPlanet(cur_ship,4096)
	if planet ~= nil then
		myx,myy = cur_ship:GetPosition()
		x,y = planet:Position()
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

