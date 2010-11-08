--- This script defines the available Missions

-- Examples of Missions include:
-- Go to Planet
-- Destroy Ship X
-- Rescue Ship X


--[[

--- The Bare Minimum information of a Mission definition.

MissionType = {
	UID = 0, --- TODO: Use this for Mission Dependencies.
	Difficulty = "EASY", --- TODO: Use this to warn Users about difficult Missions.
	Create = function() --- Call this when the Mission is created.
		--- Return a mission table.
		--- This is persistent and will be passed back to the action functions below.
		return defaultMissionTable( "Name", "Description" )
	end,
	Accept = function( missionTable ) end, --- Call this when the Mission is accepted.
	Reject = function( missionTable ) end, --- Call this when the Mission is rejected after being accepted.
	Update = function( missionTable ) --- Call this each time that the Mission should be checked.
		return nil --- Return nil when the mission isn't over yet.
		return true --- Return true when the mission has succeded.
		return false --- Return false when the mission has failed.
	end,
	Success = function( missionTable ) end, --- Call this if the Mission is a Success.
	Failure = function( missionTable ) end, --- Call this if the Mission is a failure.
}
	
]]

--- These are the bare minimum values that need to be in the each Mission Table
function defaultMissionTable( Name, Description)
	local missionTable = {
		Name = Name, 
		Description = Description,
		}
	return missionTable
end

ReturnAmbassador = {
	UID = 1, 
	Difficulty = "EASY", 
	Create = function()
		local planets = Epiar.planets()
		local n = math.random( #planets )
		local p = planets[ n ]
		local Name = string.format("Transport Ambassador to %s", p:GetName() )
		local Reward = 1000 * ( math.random(10) + 10)
		local Description = "The Ambassador of %s needs to be returned to her home planet.  She will pay you %d credits if you can get her there safely."
		Description = Description:format( p:GetName(), Reward )
		-- Save the mission information into a table
		local missionTable = defaultMissionTable( Name, Description )
		missionTable['planet'] = p:GetName()
		missionTable['reward'] = Reward
		return missionTable
	end,
	Accept = function( missionTable )
		local p = Planet.Get( missionTable.planet )
		local qx, qy = coordinateToQuadrant( p:GetPosition() )
		HUD.newAlert( string.format("Please take me to %s in the Quadrant (%d,%d)", missionTable.planet, qx, qy ) )
	end,
	Reject = function( missionTable )
		HUD.newAlert( string.format("Thanks for the help, just drop me off at your next landing" ) )
	end,
	Update = function( missionTable )
		local x,y = PLAYER:GetPosition()
		local p = Planet.Get( missionTable.planet )
		local px,py = p:GetPosition()
		if distfrom(px,py,x,y) < 50 then
			return true
		end
	end,
	Success = function( missionTable )
		HUD.newAlert(string.format("Thank you for returning me to my home.") )
		addcredits(  missionTable.reward )
	end,
	Failure = function( missionTable ) 
	end,
}

DestroyPirate = {
	UID = 1,
	Difficulty = "MEDIUM",
	Create = function()
		local missionTable = {}
		local name = choose( {"Robert", "Bob", "Joe", "Steve", "Mary", "Bart", "Paine", "John", "Jack", "Cervantes", "Sally"} )
		local title = choose( {"Red", "Black", "Yellow", "Savage", "Blood Thirtsy", "Wanderer", "Villianous", "Scruffy Looking", "Shady", "Hungry", "Illicit"} )
		missionTable.piratename = name .." The ".. title
		missionTable.Name = string.format("Destroy %s", missionTable.piratename)
		missionTable.reward = 1000 * ( math.random(10) + 20)
		local x,y = PLAYER:GetPosition()
		--x,y = x + about(4096), y + about(4096)
		missionTable.quadrant = { coordinateToQuadrant(x,y) }
		missionTable.Description = string.format( "The pirate %s has been terrorizing the (%d,%d)Quadrant.  If you can find and destroy %s we will pay you %d credits.",
				missionTable.piratename,
				missionTable.quadrant[1], missionTable.quadrant[2],
				name,
				missionTable.reward)
		missionTable.ship = nil
		return missionTable
	end,
	Accept = function( missionTable )
		local X,Y = QuadrantToCoordinate( missionTable.quadrant[1], missionTable.quadrant[2] )
		X,Y = X + about(4096), Y + about(4096)
		local model = choose( {"Fleet Guard", "Uber", "Terran Corvert Mark I", "Terran XV", "Terran Assist"} )
		local engine = "Ion Engines"
		local plan = "Pirate"
		local cur_ship = Ship.new(missionTable.Name,X,Y,model,engine,plan,"Independent")
		local weapon = choose( {"Missile", "Neon Flares", "Plasma Cannon", "Strong Laser"})
		cur_ship:AddWeapon( weapon )
		cur_ship:AddAmmo( weapon, 100 )
		for o = 1,math.random(3) do
			cur_ship:AddOutfit( choose({"Steel Plating", "Booster", "Shield Generator", "Directional Thrusters", "Titanium Plating"}) )
		end
		missionTable.ship = cur_ship:GetID()
	end,
	Reject = function( missionTable )
		
	end,
	Update = function( missionTable )
		if missionTable.ship == nil then
			return false -- Error
		else
			local cur_ship = Epiar.getSprite( missionTable.ship )
			if cur_ship == nil then
				return true
			else
				-- The pirate exists
			end
		end
	end,
	Success = function( missionTable )
		HUD.newAlert(string.format("Thank you for destroying %s!", missionTable.piratename) )
		addcredits(  missionTable.reward )
	end,
	Failure = function( missionTable )
		HUD.newAlert("This should never happen")
	end,
}



