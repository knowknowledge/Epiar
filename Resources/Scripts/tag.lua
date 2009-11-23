--------------------------------------------------------------------------------
-- Game of Tag

it = {}
it.setup = function()
    -- Setup Tag variables
    it.ship =0
    it.countdown=100
    it.pic = UI:newPicture(60,30,100,30, Ship.GetModelName(Epiar.player()) )
    it.label = UI:newLabel(90,100,"You're It")

    -- Create Tag Windows
    menuWin = UI:newWindow( 900,200,120,250,"Menu",
        UI:newButton(10,40,100,30,"Pause","pauseMessage('You hit the pause button')"),
        UI:newButton(10,140,100,30,"IT","it.tag(0)"),
        UI:newButton(10,190,100,30,"NOT IT","it.tag(math.random(#(Epiar.ships())))")
        )
    tagWin = UI:newWindow( 840,500,180,130,"Who's IT?",
        it.pic,
        it.label
        )
end
it.findClosest = function()
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	it.target=-1
	it.target_dist= 100000
	it.target_x,target_y= 10000,10000
	it.x,it.y = Ship.GetPosition(ships[it.ship])

	-- Find the closest ship to whomever is IT
	for other=1, #ships do 
		if other ~= it.ship then
			other_x,other_y = Ship.GetPosition(ships[other])
			dist = distfrom(other_x,other_y,it.x,it.y)
			if dist < it.target_dist then
				it.target = other
				it.target_dist= dist
				it.target_x,it.target_y = other_x,other_y
			end
		end
	end
	-- io.write(string.format("Closest Ship to (%d,%d): Ship #%d is at (%d,%d) %d clicks away.\n", it.x,it.y, it.target,it.target_x,it.target_y,it.target_dist))
end
it.tag = function(target)
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	if target >= #ships then
		return 1
	end
	-- the old IT now runs
	AIPlans[it.ship] = {}
	AIPlans[it.ship].time=20
	AIPlans[it.ship].plan=aimAwayFromIT
	Ship.SetRadarColor(ships[it.ship],0,255,0)
	
	--shake the camera
	if it.ship == 0 then
		other_x,other_y = Ship.GetPosition(ships[target])
		Epiar.shakeCamera(100, 3, other_x,other_y)
	elseif target == 0 then
		Epiar.shakeCamera(100, 3, it.x,it.y)
	end

	-- The new it doesn't become active for 100 ticks
	it.ship=target
	it.countdown = 100
	AIPlans[it.ship] = {}
	AIPlans[it.ship].time=0
	AIPlans[it.ship].plan=chaseClosest
	Ship.SetRadarColor(ships[it.ship],255,0,0)
end
it.UpdateIT = function()
	ships = Epiar.ships()
	ships[0] = Epiar.player()
	-- Set the Who's It? Dashboard to the correct Image
	UI.setPicture(it.pic, Ship.GetModelName(ships[it.ship]) )
	UI.rotatePicture(it.pic, Ship.GetAngle(ships[it.ship]) )

	if it.countdown==0 then
		-- Show who's it
		if it.ship==0 then
			UI.setText(it.label,"You're IT!")
		else
			UI.setText(it.label,"Player "..(it.ship).." is IT!")
		end
		-- Is someone else it now?
		if it.target_dist < 200 then 
			it.tag(it.target)
		end
	else
		it.countdown= (it.countdown)-1
		-- Update the countdown only every 10th tick
		if it.countdown%10==0 then
			if it.ship==0 then
				UI.setText(it.label,"You're IT in: "..(it.countdown/10))
			else
				UI.setText(it.label,"Player "..(it.ship).." is IT in: "..(it.countdown/10))
			end
		end
	end
end

function chaseClosest(cur_ship,timeleft)
	-- This is only done by the ship that is IT
	Ship.Rotate(cur_ship, 
		Ship.directionTowards(cur_ship, it.target_x, it.target_y) )
	Ship.Accelerate(cur_ship )
end

function aimTowardsIT(cur_ship,timeleft)
	-- direction towards the ship that is IT
	Ship.Rotate(cur_ship,
		Ship.directionTowards(cur_ship, it.x, it.y) )
	if timeleft %2 == 0 then
		Ship.Accelerate(cur_ship )
	end
end

function aimAwayFromIT(cur_ship,timeleft)
	-- direction away from the ship that is IT
	Ship.Rotate(cur_ship,
		-Ship.directionTowards(cur_ship, it.x, it.y) )
	Ship.Accelerate(cur_ship )
end

-- Register Tag
registerInit(it.setup)
registerPlan(aimTowardsIT)
registerPlan(aimAwayFromIT)
registerPreStep(it.findClosest)
registerPostStep(it.UpdateIT)


