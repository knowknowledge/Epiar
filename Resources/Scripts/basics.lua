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

function chasePlayer(cur_ship,timeleft)
	x,y = Ship.GetPosition( Epiar.player() )
	Ship.Rotate(cur_ship,
		Ship.directionTowards(cur_ship, x, y) )
	Ship.Accelerate(cur_ship )
end

function zigzag(cur_ship,timeleft)
	-- Change direction rapidly
	if timeleft % 10 <=3 then
		Ship.Rotate(cur_ship, 1)
	elseif timeleft % 10 >=7 then
		Ship.Rotate(cur_ship, -1)
	end
	Ship.Accelerate(cur_ship )
end

-- Register the Basics
registerPlan(zigzag)
registerPlan(chasePlayer)

