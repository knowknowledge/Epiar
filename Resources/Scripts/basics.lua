--------------------------------------------------------------------------------
-- Basic Scenario

function aimCenter(cur_ship,timeleft)
	-- direction towards the center or the universe
	if timeleft%3 ==0 then
		Ship.Rotate(cur_ship,
			Ship.directionTowards(cur_ship, 0,0) )
	end
	Ship.Accelerate(cur_ship )
end

function chaseClosure(targetShip)
	function plan(cur_ship,timeleft)
		x,y = Ship.GetPosition( targetShip )
		Ship.Rotate(cur_ship,
			Ship.directionTowards(cur_ship, x, y) )
		Ship.Accelerate(cur_ship )
	end
	return plan 
end

function fleeClosure(targetShip)
	function plan(cur_ship,timeleft)
		x,y = Ship.GetPosition( targetShip )
		Ship.Rotate(cur_ship,
			-Ship.directionTowards(cur_ship, x, y) )
		Ship.Accelerate(cur_ship )
	end
	return plan 
end

function fleePoint(x,y)
	function flee(cur_ship,timeleft)
		Ship.Rotate(cur_ship,
			-Ship.directionTowards(cur_ship, x, y) )
		Ship.Accelerate(cur_ship )
	end
	return plan
end

chasePlayer = chaseClosure( Epiar.player() )
chasePlayer = fleeClosure( Epiar.player() )

-- Zig, Then Zag
function zigzag(cur_ship,timeleft)
	-- Change direction rapidly
	if timeleft % 10 <=3 then
		Ship.Rotate(cur_ship, 1)
	elseif timeleft % 10 >=7 then
		Ship.Rotate(cur_ship, -1)
	end
	if timeleft % 10 == 5 then
		Ship.ChangeWeapon(cur_ship)
	end
	Ship.Fire(cur_ship )
	Ship.Accelerate(cur_ship )
end

function evacuate(cur_ship, timeleft)
	x,y = Ship.GetPosition(cur_ship)
	for s =1,number_of_ships do
		cur_ship = Ship:new(
				math.random(1000)-500+X, -- X
				math.random(1000)-500+Y, -- Y
				shiptypes[math.random(#shiptypes)],
				"chase"                 -- Ship Script
				)
		AIPlans[ Ship.GetID(cur_ship) ] = newPlan()
	end
end

function evacuateCheck(percent)
	function evacuate()
		ships = Epiar.ships()
		-- Move Non-Player ships
		for s =1, #ships do
			cur_ship = ships[s]
			n = Ship.GetID(cur_ship)
			--io.write("Ship["..n.."] ("..Ship.GetModelName(cur_ship)..")"..Ship.GetHull(cur_ship).."\n")
			if (Ship.GetHull(cur_ship) < percent) and (Ship.GetModelName(cur_ship) ~= "Escape Pod" )then
				io.write("Escape!\n")
				x,y = Ship.GetPosition(cur_ship)
				Ship.Explode(cur_ship)
				for pod = 1,10 do
					cur_ship = Ship:new(
							math.random(10)-5+x, -- X
							math.random(10)-5+y, -- Y
							"Escape Pod",
							"Escaping"                 -- Ship Script
							)
					AIPlans[ Ship.GetID(cur_ship) ] = {plan=fleePoint(x,y),time=100}
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
			n = Ship.GetID(cur_ship)
			if AIPlans[n].time < 3 then
				x,y = Ship.GetPosition(cur_ship)
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

