--- This script defines the available Missions

-- Examples of Missions include:
-- Go to Planet
-- Destroy Ship X
-- Rescue Ship X


--

--- These are the bare minimum values that need to be in the each Mission Table
function defaultMissionTable( Name, Description)
	local missionTable = {
		Name = Name, 
		Description = Description,
		}
	return missionTable
end

ReturnAmbassador = {
	UID = 1, --- TODO: Use this for Mission Dependencies
	Difficulty = "EASY", --- TODO: Use this to warn Users about difficult Missions
	--- Call this when the Mission is created.
	Create = function()
		local planets = Epiar.planets()
		local n = math.random( #planets )
		local p = planets[ n ]
		local Name = string.format("Transport Ambassador to %s", p:GetName() )
		local Reward = 1000 * ( math.random(10) + 10)
		local Description = "The Ambassador of %s needs to be returned to her home planet.  She will pay you %d credits if you can get her there safely."
		Description = string.format(Description, p:GetName(), Reward )
		-- Save the mission information into a table
		local missionTable = defaultMissionTable( Name, Description )
		missionTable['planet'] = p:GetName()
		missionTable['reward'] = Reward
		return missionTable
	end,
	--- Call this when the Mission is accepted.
	Accept = function( missionTable )
	end,
	--- Call this each time that the Mission should be checked.
	--- Update Returns true on Success, False on Failure.  Normally this does not return anything.
	Update = function( missionTable )
		local x,y = PLAYER:GetPosition()
		local p = Planet.Get( missionTable.planet )
		local px,py = p:GetPosition()
		if distfrom(px,py,x,y) < 50 then
			return true
		end
	end,
	--- Call this if the Mission is a Success
	Success = function( missionTable )
		HUD.newAlert(string.format("Thank you for returning me to my home.") )
		addcredits(  missionTable.reward )
	end,
	--- Call this if the Mission is a failure
	Failure = function( missionTable ) 
	end,
}




