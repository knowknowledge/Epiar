--
-- Swarm 

--- The swarm is the net average of all ships
function newSwarm()
	swarm = {}
	swarm.avg_x= 0
	swarm.avg_y= 0
	swarm.avg_angle = 0
	swarm.avg_vector= 0
	swarm.avg_speed = 0
	return swarm
end

--- Swarm average
function swarmAverage(x,y,r)
	swarm = newSwarm()
	-- Average the statistics of each ship
	ships = Epiar.ships()
	for s =1,#ships do
		cur_ship = ships[s]
		x,y = cur_ship:GetPosition()
		swarm.avg_x = swarm.avg_x + x
		swarm.avg_y = swarm.avg_y + y
		swarm.avg_angle = swarm.avg_angle + cur_ship:GetAngle()
		swarm.avg_vector= swarm.avg_vector+ cur_ship:GetMomentumAngle()
		swarm.avg_speed = swarm.avg_speed + cur_ship:GetMomentumSpeed()
	end
	-- Normalize
	swarm.avg_x = swarm.avg_x / #ships
	swarm.avg_y = swarm.avg_y / #ships
	swarm.avg_angle = swarm.avg_angle / #ships
	swarm.avg_vector = swarm.avg_vector / #ships
	swarm.avg_speed = swarm.avg_speed / #ships
	return swarm
end

--- AIM at swarm center
function aimSwarmCenter(cur_ship,timeleft)
	-- direction towards the center of the swarm
	x,y = cur_ship:GetPosition()
	swarm = swarmAverage(x,y,1000)
	if timeleft %2 == 0 then
		cur_ship:Rotate( cur_ship:directionTowards(swarm.avg_x, swarm.avg_y) )
	end
	cur_ship:Accelerate()
end

--- AIM in swarm direction
function aimSwarmDirection(cur_ship,timeleft)
	x,y = cur_ship:GetPosition()
	swarm = swarmAverage(x,y,1000)
	-- rotate to the same direction as the swarm
	cur_ship:Rotate( cur_ship:directionTowards(swarm.avg_angle) )
	cur_ship:Accelerate()
end

-- Register Swarm 
registerPlan(aimSwarmCenter)
registerPlan(aimSwarmDirection)
