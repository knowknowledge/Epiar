-- This script defines the available Missions
--[[

The Bare Minimum information of a Mission definition.

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
	Land = function( missionTable ) --- Call this each time that player lands
		return nil --- Return nil when the mission isn't over yet.
		return true --- Return true when the mission has succeded.
		return false --- Return false when the mission has failed.
	end,
	Success = function( missionTable ) end, --- Call this if the Mission is a Success.
	Failure = function( missionTable ) end, --- Call this if the Mission is a failure.
}

--]]

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
	Version = 2,
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
	end,
	Land = function( missionTable )
		local x,y = PLAYER:GetPosition()
		local p = Planet.Get( missionTable.planet )
		local px,py = p:GetPosition()
		if distfrom(px,py,x,y) < p:GetSize() then
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
	Land = function(missionTable)
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
	Version = 2,
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
			sentence = sentence:format( objects[i], planetNames[i % #planetNames] )
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
	end,
	Land = function( missionTable )
		local totalFound = 0
		local x,y = PLAYER:GetPosition()
		for i=1, missionTable.NumArtifacts do
			if missionTable.Collected[i] == false then
				local p = Planet.Get( missionTable.PlanetsWithArtifacts[i] )
				local px,py = p:GetPosition()
				if distfrom(px,py,x,y) < p:GetSize() then
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
	Version = 2,
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
	end,
	Land = function( missionTable )
		-- Check if the player has landed at the destination
		local x,y = PLAYER:GetPosition()
		local p = Planet.Get( missionTable.Planet )
		local px,py = p:GetPosition()
		if distfrom(px,py,x,y) < p:GetSize() then
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

DestroyGaryTheGold = {
	UID = 4,
	Version = 1,
	Author = "Rikus Goodell",
	Difficulty = "HARD",
	Create = function()
		
		local reward = 100000 + (10000*math.random(10))
		local planets = Epiar.planets()
		local p = planets[math.random(#planets)]
		local planetName = p:GetName()
		local missionTable = defaultMissionTable( "Destroy Gary the Gold", (string.format("Insane ship captain Gary the Gold has stolen a piece of once secret military technology known as the Gold Beam. After assuming his new title and giving his ship a fitting paint job, he and his brother Larry have begun terrorizing merchants who are defenseless against their highly illegal armaments. They were last seen in the vicinity of %s. Reward is %d.", planetName, reward)) )
		missionTable.planet = planetName
		missionTable.reward = reward
		return missionTable
	end,
	Accept = function( missionTable )
		local planetName = missionTable.planet
		local p = Planet.Get( planetName )
		local X, Y = p:GetPosition()
		local garyX = X + math.random(700) - 350
		local garyY = Y + math.random(700) - 350
		local gary = Ship.new("Gary the Gold", garyX, garyY, "Golden Uber", "Ion Engines", "Pirate", "Independent")
		gary:SetRadarColor(255,0,0)
		--attachStandardWeapons(gary, Epiar.weapons())

                local escort = Ship.new("Larry the Gold",garyX-150,garyY-150, "Vespan Carrier", "Ion Engines","Escort","Independent")
		escort:RemoveWeapon("Strong Laser")
		escort:RemoveWeapon("Strong Laser")
		escort:AddWeapon("Gold Beam")
		escort:AddWeapon("Gold Beam")
		escort:SetRadarColor(255,0,0)
                setAccompany(escort:GetID(), gary:GetID())

		missionTable.garyID = gary:GetID()
		missionTable.escortID = escort:GetID()
	end,
	Reject = function( missionTable )
		HUD.newAlert( "Gary may never be stopped" )
	end,
	Update = function( missionTable )
		local gary = Epiar.getSprite( missionTable.garyID )
		local escort = Epiar.getSprite( missionTable.escortID )
		if gary == nil and escort == nil then
			return true
		else
			-- do nothing
		end
	end,
	Land = function( missionTable )
	end,
	Success = function( missionTable )
		HUD.newAlert("Thank you for destroying Gary the Gold!")
		addcredits(missionTable.reward)
	end,
	Failure = function( missionTable )
	end,

}

ProtectFreighter = {
	UID = 5,
	Version = 2,
	Author = "Rikus Goodell",
	Difficulty = "MEDIUM",
	Create = function()
		local reward = 10000 + math.random(100)*200
		local planets = Epiar.planets()
		local p = planets[math.random(#planets)]
		local planetName = p:GetName()

		local pronoun
		local maleNames = { "Manfred", "Johnny", "Otis", "Gus", "Gilbert" }
		local femaleNames = { "Suzie", "Wanda", "Francine", "Bertha", "Mildred" }
		local freighterName
		if math.random(2) == 1 then
			pronoun = "his"
			freighterName = (string.format("Captain %s", maleNames[math.random(#maleNames)]))
		else
			pronoun = "her"
			freighterName = (string.format("Captain %s", femaleNames[math.random(#femaleNames)]))
		end

		local missionTable = defaultMissionTable( (string.format ("Protect freighter en route to %s", planetName)),
		   (string.format(
		      "%s, under threat from pirates, requests that you ensure %s safe arrival at %s. Reward is %d.",
		      freighterName, pronoun, planetName, reward) )
		)
		missionTable.freighterName = freighterName
		missionTable.planet = planetName
		missionTable.reward = reward
		local pX, pY = PLAYER:GetPosition();
		missionTable.fX = pX - 100
		missionTable.fY = pY
		missionTable.playerName = PLAYER:GetName()
		return missionTable
	end,
	Accept = function( missionTable )
		local planetName = missionTable.planet
		local p = Planet.Get( planetName )
		local pX, pY = p:GetPosition()

		local createFreighter = function(type)
			-- Add freighter
			local freighter = Ship.new(
			   string.format("%s", missionTable.freighterName), missionTable.fX, missionTable.fY, "Hammer Freighter",
			   "Ion Engines", type, "Independent" )
			missionTable.freighter = freighter:GetID()
			freighter:SetRadarColor(0,255,0)
			HUD.newAlert( (string.format("%s: \"Thank you for agreeing to help, %s\"",
			   missionTable.freighterName, missionTable.playerName ) ) )
			missionTable.joined = false

			-- Add pirates
			local pirateX = pX + math.random(7000) - 3500
			local pirateY = pY + math.random(7000) - 3500
			local pirate = Ship.new("Pete", pirateX, pirateY, "Terran Corvert Mark I", "Ion Engines", "Pirate", "Independent")
			pirate:SetRadarColor(255,0,0)
			local escort = Ship.new("Skip",pirateX-150,pirateY-150, "Fleet Guard", "Ion Engines","Escort","Independent")
			escort:SetRadarColor(255,0,0)
			setAccompany(escort:GetID(), pirate:GetID())
			setHuntHostile(pirate:GetID(), freighter:GetID())
		end
		createFreighter("Escort")
	end,
	Reject = function( missionTable )
		HUD.newAlert( (string.format("%s: \"%s! I find your betrayal most disappointing.\"", missionTable.freighterName, PLAYER:GetName() ) ) )
		setAccompany(missionTable.freighter, -1)
		Fleets:unjoin( PLAYER:GetID(), missionTable.freighter )
	end,
	Update = function( missionTable )
		local freighter = Epiar.getSprite( missionTable.freighter )
		-- Check that the Freighter is still alive
		if freighter ~= nil then
			return false
		end
	end,
	Land = function( missionTable )
		local freighter = Epiar.getSprite( missionTable.freighter )
		local p = Planet.Get( missionTable.planet )

		if missionTable.joined == false then
			local id = missionTable.freighter
			Fleets:join(PLAYER:GetID(), id)
			missionTable.joined = true
		end

		if freighter ~= nil and p ~= nil then
			local fX, fY = freighter:GetPosition()
			local pX, pY = p:GetPosition()
			if distfrom( fX, fY, pX, pY ) < p:GetSize() then
				return true
			end
			missionTable.fX = fX
			missionTable.fY = fY
		else
			return false
		end
	end,
	Success = function( missionTable )
		HUD.newAlert("Mission succeeded.")
		addcredits(missionTable.reward)
		setAccompany(missionTable.freighter, -1)
		Fleets:unjoin( PLAYER:GetID(), missionTable.freighter )
	end,
	Failure = function( missionTable )
		HUD.newAlert( (string.format("%s was destroyed! Mission failed.", missionTable.freighterName) ) )
	end,

}
