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
		x,y = targetShip:GetPosition()
		cur_ship:Rotate( cur_ship:directionTowards(x, y) )
		cur_ship:Accelerate()
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

chasePlayer = chaseClosure( PLAYER )
chasePlayer = fleeClosure( PLAYER )

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
	cur_ship:Fire()
	cur_ship:Accelerate()
end

function evacuate(cur_ship, timeleft)
	x,y = cur_ship:GetPosition()
	for s =1,number_of_ships do
		cur_ship = Ship.new(
				math.random(1000)-500+X, -- X
				math.random(1000)-500+Y, -- Y
				shiptypes[math.random(#shiptypes)],
				"chase"                 -- Ship Script
				)
		AIPlans[ cur_ship:GetID() ] = newPlan()
	end
end

function evacuateCheck(percent)
	function evacuate()
		ships = Epiar.ships()
		-- Move Non-Player ships
		for s =1, #ships do
			cur_ship = ships[s]
			n = cur_ship:GetID()
			if (cur_ship:GetHull() < percent) and (cur_ship:GetModelName() ~= "Escape Pod" )then
				io.write("Escape!\n")
				x,y = cur_ship:GetPosition()
				cur_ship:Explode()
				for pod = 1,10 do
					cur_ship = Ship.new(
							math.random(10)-5+x, -- X
							math.random(10)-5+y, -- Y
							"Escape Pod",
							"Escaping"                 -- Ship Script
							)
					AIPlans[ cur_ship:GetID() ] = {plan=fleePoint(x,y),time=100}
				end
			end
		end
	end
	return evacuate
end

function moreTraffic(tickcycle)
	ticks = tickcycle
	function traffic()
		ticks = ticks -1
		if ticks == 0 then
			io.write("More Traffic!")
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
registerPlan(zigzag)
registerPlan(chasePlayer)
registerPostStep(evacuateCheck(.10))
registerPostStep(boundingClosure(2000,300))
registerPostStep(moreTraffic(1000))

