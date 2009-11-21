--------------------------------------------------------------------------------
-- Swarm 

-- The swarm is the net average of all ships
swarm = {}
swarm.reset = function()
	swarm.avg_x= 0
	swarm.avg_y= 0
	swarm.avg_angle = 0
	swarm.avg_vector= 0
	swarm.avg_speed = 0
end
swarm.findAverage = function()
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	swarm.reset()
	-- Average the statistics of each ship
	for s =1,#ships do
		cur_ship = ships[s]
		x,y = Ship.GetPosition(cur_ship)
		swarm.avg_x = swarm.avg_x + x
		swarm.avg_y = swarm.avg_y + y
		swarm.avg_angle = swarm.avg_angle + Ship.GetAngle(cur_ship)
		swarm.avg_vector= swarm.avg_vector+ Ship.GetMomentumAngle(cur_ship)
		swarm.avg_speed = swarm.avg_speed + Ship.GetMomentumSpeed(cur_ship)
	end
	-- Normalize
	swarm.avg_x = swarm.avg_x / #ships
	swarm.avg_y = swarm.avg_y / #ships
	swarm.avg_angle = swarm.avg_angle / #ships
	swarm.avg_vector = swarm.avg_vector / #ships
	swarm.avg_speed = swarm.avg_speed / #ships
end

function aimSwarmCenter(cur_ship,timeleft)
	-- direction towards the center of the swarm
	if timeleft %2 == 0 then
		Ship.Rotate(cur_ship, 
			Ship.directionTowards(cur_ship, swarm.avg_x, swarm.avg_y) )
	end
	Ship.Accelerate(cur_ship )
end

function aimSwarmDirection(cur_ship,timeleft)
	-- rotate to the same direction as the swarm
	Ship.Rotate(cur_ship,
		Ship.directionTowards(cur_ship, swarm.avg_angle) )
	Ship.Accelerate(cur_ship )
end

-- Register Swarm 
registerInit(swarm.reset)
registerPlan(aimSwarmCenter)
registerPlan(aimSwarmDirection)
registerPreStep(swarm.findAverage)
