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
	x,y = Ship.GetPosition( targetShip )
	io.write("("..x..","..y..")\n")
	function chase(cur_ship,timeleft)
		x,y = Ship.GetPosition( targetShip )
		io.write("("..x..","..y..")\n")
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
			x,y = Ship.GetPosition(cur_ship)
			if distfrom(x,y,0,0) >distance then
				AIPlans[s] = {plan=aimCenter,time=ticks}
			end
		end
	end
	return boundingBox
end

-- Register the Basics
registerPlan(zigzag)
registerPlan(chasePlayer)
registerPreStep(boundingClosure(1000,300))

