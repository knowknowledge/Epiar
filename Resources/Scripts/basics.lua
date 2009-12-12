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
	function chase(cur_ship,timeleft)
		x,y = Ship.GetPosition( targetShip )
		Ship.Rotate(cur_ship,
			Ship.directionTowards(cur_ship, x, y) )
		Ship.Accelerate(cur_ship )
	end
	return chase
end

chasePlayer = chaseClosure( Epiar.player() )

-- Zig, Then Zag
function zigzag(cur_ship,timeleft)
	-- Change direction rapidly
	if timeleft % 10 <=3 then
		Ship.Rotate(cur_ship, 1)
	elseif timeleft % 10 >=7 then
		Ship.Rotate(cur_ship, -1)
	end
	Ship.Accelerate(cur_ship )
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
registerPostStep(boundingClosure(4000,300))

