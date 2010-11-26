--- This script defines the available Missions

-- Examples of Missions include:
-- Go to Planet
-- Destroy Ship X
-- Rescue Ship X
-- TraderProtection


--[[

--- The Bare Minimum information of a Mission definition.

MissionType = {
	UID = 0, --- TODO: Use this for Mission Dependencies.
	Version = 1, --- Increment this whenever changes are made to the Mission.
	Author = "", --- The name of the first author.
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
	Version = 1,
	Author = "Matt Zweig",
	Difficulty = "EASY",
	Create = function()
		local p = choose( Epiar.planets() )
		local professions = {"Ambassador", "Smuggler", "Executive", "Trader"}
		local profession = choose( professions )
		local Name = string.format("Transport %s to %s", profession, p:GetName() )
		local Reward = 1000 * ( math.random(10) + 10)
		local Description = "A %s %s needs to be returned to %s. You will be paid %d credits if you can get there safely."
		local alliance = choose( Epiar.alliances() )
		Description = Description:format( alliance, profession, p:GetName(), Reward )
		-- Save the mission information into a table
		local missionTable = defaultMissionTable( Name, Description )
		missionTable.planet = p:GetName()
		missionTable.reward = Reward
		missionTable.alliance = alliance
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
	UID = 2,
	Version = 1,
	Author = "Matt Zweig",
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


CollectArtifacts = {
	UID = 3,
	Version = 1,
	Author = "Matt Zweig",
	Difficulty = "EASY",
	Create = function()
		local missionTable = defaultMissionTable(
				"Collect %d Artifacts from %s for The %s",
				"Several important items of %s have been found by %s %s.  The %s will pay you %d credits to return them safely to %s."
			)

		-- The vocabulary of our mission madlibs
		local allianceNames = Epiar.alliances()
		local planetNames = Epiar.planetNames()
		local objects = {"Sword", "Blaster", "Helm", "Amulet", "Tome", "Flag", "Statue", "History", "MacGuffin", "Crystal", "Book", "Document"}
		local adjectives = {"Red", "Black", "Bloodless", "Steel", "Iron", "Circular", "Mistaken", "Forgotten", "Wild", "Nuclear"}
		local events = {"Revolution", "Revolt", "Insurgency", "Tournament", "Race", "Decade", "Legend", "Treaty", "Project", "Council"}
		local actors = {"revolutionaries", "spies", "rebels", "agitators", "freedom fighters", "terrorists", "hooligans", "mobsters", "pirates", "officials", "assassins"}

		-- The rest of the Mission Table entries.
		missionTable.EventName = "The %s %s"
		missionTable.Actors = choose( actors )
		missionTable.FriendAlliance = choose( allianceNames )
		missionTable.EnemyAlliance = choose( allianceNames )
		missionTable.NumArtifacts = math.random( 2, 6 )
		missionTable.FinalPlanet = choose( planetNames )
		missionTable.Adjective = choose( adjectives )
		missionTable.Event = choose( events )
		missionTable.Reward = 5000 + (missionTable.NumArtifacts * 2000)
		missionTable.Objects = {}
		missionTable.Collected = {}
		missionTable.PlanetsWithArtifacts = {}

		-- Fill in the blanks madlib style
		missionTable.EventName = missionTable.EventName:format( missionTable.Adjective, missionTable.Event )
		missionTable.Name = missionTable.Name:format( missionTable.NumArtifacts, missionTable.EventName, missionTable.FriendAlliance)
		missionTable.Description = missionTable.Description:format( missionTable.EventName, missionTable.EnemyAlliance, missionTable.Actors, missionTable.FriendAlliance, missionTable.Reward, missionTable.FinalPlanet)

		-- Get a random subset of the Objects and Planets
		table.shuffle( planetNames )
		table.shuffle( objects )
		for i=1, missionTable.NumArtifacts do
			table.insert( missionTable.PlanetsWithArtifacts, planetNames[i] )
			table.insert( missionTable.Objects, objects[i] )
			table.insert( missionTable.Collected, false )
			local sentence = "  The %s can be found on %s."
			sentence = sentence:format( objects[i], planetNames[i] )
			missionTable.Description = missionTable.Description .. sentence
		end

		return missionTable
	end,
	Accept = function( missionTable )
		local acceptMessage = "The artifacts from %s can be found at %s"
		local places = missionTable.PlanetsWithArtifacts[1] .. " and " .. missionTable.PlanetsWithArtifacts[2]
		for i=3, missionTable.NumArtifacts do
			places = missionTable.PlanetsWithArtifacts[1] .. ", " .. places
		end
		acceptMessage = acceptMessage:format( missionTable.EventName, places )
		HUD.newAlert( acceptMessage  )
	end,
	Reject = function( missionTable )
		local rejectMessage = "The %s %s will get away with the artifacts."
		rejectMessage = rejectMessage:format( missionTable.EnemyAlliance, missionTable.Actors )
		HUD.newAlert( acceptMessage  )
	end,
	Update = function( missionTable )
		local totalFound = 0
		local x,y = PLAYER:GetPosition()
		for i=1, missionTable.NumArtifacts do
			if missionTable.Collected[i] == false then
				local p = Planet.Get( missionTable.PlanetsWithArtifacts[i] )
				local px,py = p:GetPosition()
				if distfrom(px,py,x,y) < 50 then
					-- This artifact has been recovered
					-- Record this in the Description
					local desc = "** You have recovered the %s **"
					desc = desc:format( missionTable.Objects[i] )
					missionTable.Description = missionTable.Description .. desc
					-- Alert the Player
					local message = "You have recovered the %s of %s from %s."
					message = message:format( missionTable.Objects[i], missionTable.EventName, missionTable.PlanetsWithArtifacts[i] )
					HUD.newAlert( message )
					-- Mark this Object as Collected
					missionTable.Collected[i] = true
				end
			else
				totalFound = totalFound + 1
			end
		end
		if totalFound == missionTable.NumArtifacts then
			local p = Planet.Get( missionTable.FinalPlanet )
			local px,py = p:GetPosition()
			if distfrom(px,py,x,y) < 50 then
				local message = "All of the Artifacts from %s have been delivered to the %s on ."
				message = message:format( missionTable.EventName, missionTable.FriendAlliance, missionTable.FinalPlanet )
				HUD.newAlert( message )
				return true
			end
		end
	end,
	Success = function( missionTable )
		addcredits(  missionTable.Reward )
	end,
	Failure = function( missionTable ) end,
}

ShippingRoutes = {
	UID = 5,
	Version = 1,
	Author = "Matt Zweig",
	Difficulty = "EASY",
	Create = function()
		local missionTable = {}
		local planet = choose( Epiar.planets() )

		missionTable.Name = "Transport %d tons of %s to %s"
		missionTable.Description = "The planet of %s needs high quality %s.  A Trader will pay you %d to deliver %d tons of his %s."
		missionTable.Tonnage = math.random(50) + 10
		missionTable.Commodity = choose( Epiar.commodities() )
		missionTable.Planet = planet:GetName()
		missionTable.Reward = 1000 + (100 * missionTable.Tonnage)

		missionTable.Name = missionTable.Name:format( missionTable.Tonnage, missionTable.Commodity, missionTable.Planet )
		missionTable.Description = missionTable.Description:format( missionTable.Planet, missionTable.Commodity, missionTable.Reward, missionTable.Tonnage, missionTable.Commodity )

		return missionTable
	end,
	Accept = function( missionTable )
		local message = "%d tons of %s have been stored in your hold."
		local stored = PLAYER:StoreCommodities( missionTable.Commodity, missionTable.Tonnage )
		if(stored ~= missionTable.Tonnage) then
			PLAYER:DiscardCommodities( missionTable.Commodity, stored )
			message = "You don't have enough space to store %d tons of %s."
		end
		message = message:format( missionTable.Tonnage, missionTable.Commodity )
		HUD.newAlert( message )
	end,
	Reject = function( missionTable )
		PLAYER:DiscardCommodities( missionTable.Commodity, missionTable.Tonnage )
		local message = "You've jettisoned the %d tons of %s destined for %s."
		message = message:format( missionTable.Tonnage, missionTable.Commodity, missionTable.Planet )
		HUD.newAlert( message )
	end,
	Update = function( missionTable )
		-- Check if the Player still has all the cargo
		local currentCargo, stored, storable = PLAYER:GetCargo()
		if currentCargo[ missionTable.Commodity ] < missionTable.Tonnage then
			return false
		end

		-- Check if the player has landed at the destination
		local x,y = PLAYER:GetPosition()
		local p = Planet.Get( missionTable.Planet )
		local px,py = p:GetPosition()
		if distfrom(px,py,x,y) < 50 then
			return true
		end
	end,
	Success = function( missionTable )
		addcredits(  missionTable.Reward )
		PLAYER:DiscardCommodities( missionTable.Commodity, missionTable.Tonnage )
		local message = "You've safely delivered %d tons of %s to %s."
		message = message:format( missionTable.Tonnage, missionTable.Commodity, missionTable.Planet )
		HUD.newAlert( message )
	end,
	Failure = function( missionTable )
		-- Discard remaining cargo.
		local message = "You've lost the job to deliver %s to %s."
		message = message:format( missionTable.Commodity, missionTable.Planet )
		HUD.newAlert( message )
	end,
}

