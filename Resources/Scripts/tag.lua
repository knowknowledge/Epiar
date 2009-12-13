--------------------------------------------------------------------------------
-- Game of Tag

it = {}
it.setup = function()
    -- Setup Tag variables
    it.ship = Epiar.player()
    it.shipID = Ship.GetID( it.ship )
    it.countdown=100
    it.pic = UI:newPicture(0,0,100,30, Ship.GetModelName(Epiar.player()) )
    it.label = UI:newLabel(90,80,"You're It",1)

    -- Create Tag Windows
    menuWin = UI:newWindow( 900,200,110,250,"Menu",
        UI:newButton(0,0,100,30,"Pause","pauseMessage('You hit the pause button')"),
        UI:newButton(0,40,100,30,"Store","store()"),
        UI:newButton(0,80,100,30,"IT","it.tag(Epiar.player())")
        --UI:newButton(0,120,100,30,"NOT IT","it.tag(math.random(#(Epiar.ships())))")
        )
    tagWin = UI:newWindow( 840,500,180,130,"Who's IT?",
        it.pic,
        it.label
        )
end
it.findClosest = function()
	if nil == it.ship then io.write("No one is IT!") return end
	it.x,it.y = Ship.GetPosition(it.ship)

	-- Find the closest ship to whomever is IT
	closeShips = Epiar.ships( it.x, it.y, 1000)
	if #closeShips < 2 then return end
	if it.shipID == Ship.GetID(closeShips[1]) then
		it.target = closeShips[2]
	else
		it.target = closeShips[1]
	end
	it.targetID = Ship.GetID( it.target )
	it.target_x,it.target_y = Ship.GetPosition( it.target )
	it.target_dist= distfrom(it.target_x,it.target_y, it.x,it.y)
	io.write(string.format("Closest Ship to (%d,%d): Ship #%d is at (%d,%d) %d clicks away.\n", it.x,it.y, it.targetID,it.target_x,it.target_y,it.target_dist))
end
it.tag = function(target)
	-- the old IT now runs
	AIPlans[it.shipID] = {}
	AIPlans[it.shipID].time=20
	AIPlans[it.shipID].plan=aimAwayFromIT
	Ship.SetRadarColor(it.ship,0,255,0)
	
	-- The new it doesn't become active for 100 ticks
	it.ship=target
	it.shipID = Ship.GetID(target)
	io.write("Ship #"..it.shipID.." is now it.\n")
	it.countdown = 10
	AIPlans[it.shipID] = {}
	AIPlans[it.shipID].time=0
	AIPlans[it.shipID].plan=chaseClosest
	Ship.SetRadarColor(it.ship,255,0,0)
end
it.UpdateIT = function()
	-- Set the Who's It? Dashboard to the correct Image
	UI.setPicture(it.pic, Ship.GetModelName(it.ship) )
	UI.rotatePicture(it.pic, Ship.GetAngle(it.ship) )

	if it.countdown==0 then
		-- Show who's it
		if it.ship==Epiar.player() then
			UI.setText(it.label,"You're IT!")
		else
			UI.setText(it.label,"Player "..(it.shipID).." is IT!")
		end
		-- Is someone else it now?
		if it.target_dist < 200 then 
			it.tag(it.target)
		end
	else
		it.countdown= (it.countdown)-1
		-- Update the countdown only every 10th tick
		if it.countdown%10==0 then
			if it.ship==Epiar.player() then
				UI.setText(it.label,"You're IT in: "..(it.countdown/10))
			else
				UI.setText(it.label,"Player "..(it.shipID).." is IT in: "..(it.countdown/10))
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


