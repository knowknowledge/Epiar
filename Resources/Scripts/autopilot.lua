--
-- Autopilot for Epiar player
--   Last updated: Nov 29, 2010
--
-- some revision history is in RCS - last RCS revision:
--   $Id: autopilot.lua,v 1.10 2010/11/28 23:22:13 rikus Exp $
--
--   This is an automatic gate navigation system for the player. It might make sense to
--   turn it into an AI and allow the player to be controlled by that AI. Then it could
--   also guide escorts, for example, through the gates with you. However, a case like that
--   it might make it tempting for the autopilot to recompute shortest path every time the player
--   moves, which would be unacceptably CPU intensive, so a better way of tracking the player
--   would need to be devised.
--
-- How to use it:
--   0) buy the "Nav Aid" outfit (it won't work until you do)
--   1) hit 'T' (capital) and choose your destination (must be a valid planet/station name)
--   2) hold down Left Alt and accelerate in the direction it takes you.
--   3) if you think you are headed completely in the right direction or if it tells you to stop,
--      stop accelerating, but keep Left Alt held down. (this could be automated)
--   4) when you are through the gate and it tells you to resume acceleration, start accelerating
--      again with Left Alt still held.
--   5) repeat until you arrive at your destination
--

function APInit()
	Autopilot = { }
	Autopilot.Objects = { }
	Autopilot.numObjects = 0
	Autopilot.SpatialDistances = { } 
	Autopilot.GateRoute = { }
	Autopilot.showGateRoute = function ()
		print ""
		for num =1,#Autopilot.GateRoute do
			print (string.format("   GR %d %s", num, Autopilot.GateRoute[num]))
		end
	end
	Autopilot.graphEdge = function(u, v)
		if u == nil or v == nil then
			print "Autopilot.graphEdge() received one or both values nil."
			return nil
		end
		return (string.format("%s to %s", u, v))
	end
	Autopilot.hasAutopilot = function(ship)
		for n,po in pairs( ship:GetOutfits() ) do
			if po == "Nav Aid" then return true end
		end
		return false
	end
	Autopilot.showAlert = function()
		local dest = Autopilot.GateRoute[#Autopilot.GateRoute]
		local next = Autopilot.GateRoute[1]
		HUD.newAlert( (string.format("Autopilot: engaged, en route to %s, next object is %s", dest, next) ) )
	end

end

function shortestPath(orig, dest)

	-- Sorry; these obfuscated-looking variables are to best match the algorithm description from my CS textbook!

	local d = { }  -- shortest path distances from s; e.g., d["Ves"] is the length of the shortest known route to from s to Ves.
	local pred = { } -- predecessor list; e.g., pred["Ves"] is the gate (if any -- else origin) that the player exits before reaching Ves
	local s = orig -- starting point (most likely "player")
	local V = Autopilot.Objects -- names of planets, stations, and gates
	local S = { }
	local l = Autopilot.SpatialDistances -- edge lengths: lengths between partner gates are zero, all others are calculated by distfrom().

	-- relax distances between edges based on newly discovered routes
	local relax = function ( u, v )
		if u == v then return end
		local e_uv = Autopilot.graphEdge(u, v)
		local luv = l[e_uv]
		-- if luv is nil, then this may be a planet-planet edge; we don't care about those
		if luv == nil then return end
		-- for this algorithm, d[x] == nil shall be taken to mean d(x) = infinity (unexplored); not to be confused with d(x) = 0
		if d[u] == nil then return end
		-- if d(v) is infinite or longer than d(u) + l(u,v), then set d(v) = d(u) = l(u,v), and set predecessor(v) = u
		if d[v] == nil or d[v] > d[u] + luv then
			d[v] = d[u] + luv
			pred[v] = u
		end
	end

	-- initialize with values for starting point
	d[s] = 0	-- distance to s is 0
	S[s] = true	-- s is the only node that has been explored
	pred[s] = nil	-- s has no predecessor

	local relaxations = 0
	local cycles = 0

	-- My original plan was to implement Dijkstra's algorithm, but I don't know of a priority queue
	-- structure in Lua, and I seem to have better luck with Bellman-Ford. Running time of O(nm)
	-- for n = objs and m = edges is worse than Dijkstra. Ways the running time could be reduced:
	--     - switch to Dijkstra
	--     - only iterate through Gate-Gate and Gate-Planet pairs; Planet-Planet is ignored anyway by the luv == nil test in relax(), but the loop still happens.
	--     - find a faster way of achieving "choose v from V such that v not in S" (choose v from V - S) rather than just testing for existence in S of each object

	-- Fortunately, this only has to run each time the player chooses to calculate a new route, which is rare.

	for i =1,Autopilot.numObjects-1 do -- <--- repeat (Autopilot.numObjects - 1) times
		for i1,u in pairs(V) do -- <----------\_______________ for each
			for i2,v in pairs(V) do -- <--/                edge (u,v)
				if S[v] == nil then -- such that v not in S
					if( u ~= v ) then relax( u, v ) end
					relaxations = relaxations + 1
				end
				cycles = cycles + 1
			end
		end
	end

	print ("relaxations: "..relaxations)
	print ("cycles: "..cycles)

	if dest == s then
		-- this should not happen unless the player asks the for a route to "player"
	else
		Autopilot.buildRoute = function(obj)
			table.insert(Autopilot.GateRoute, 0, obj)
			if obj == orig or obj == nil then return true end
			return Autopilot.buildRoute(pred[obj])
		end

		Autopilot.buildRoute(dest)
	end
	Autopilot.showGateRoute()
end

function calculateSpatialDistances ()
	local theGates = Epiar.gates()
	local thePlanets = Epiar.planets()
	local playerX, playerY = PLAYER:GetPosition()
	Autopilot.numObjects = 0
	for num,gate1 in pairs(theGates) do
		local gi1 = Epiar.getGateInfo(gate1:GetID())

		-- loop to calculate spatial distances between all gate pairs (including partners, which are recorded as zero distance)
		for num,gate2 in pairs(theGates) do
			local gi2 = Epiar.getGateInfo(gate2:GetID())
			local edge = Autopilot.graphEdge(gi1.Name, gi2.Name)
			Autopilot.SpatialDistances[edge] = distfrom( gi1.X, gi1.Y, gi2.X, gi2.Y )
			if gi1.Exit == gi2.Name then 
				Autopilot.SpatialDistances[edge] = 0
			end
		end

		-- calculate distance from player to this gate
		local edge = Autopilot.graphEdge("player", gi1.Name)
		Autopilot.SpatialDistances[edge] = distfrom( playerX, playerY, gi1.X, gi1.Y )
		edge = Autopilot.graphEdge(gi1.Name, "player")
		Autopilot.SpatialDistances[edge] = distfrom( gi1.X, gi1.Y, playerX, playerY )

		-- loop to calculate distances Gate-Planet and player-Planet
		for num,planet in pairs(thePlanets) do
			local pi = Epiar.getPlanetInfo(planet:GetID())

			local edge = Autopilot.graphEdge(gi1.Name, pi.Name)
			Autopilot.SpatialDistances[edge] = distfrom( gi1.X, gi1.Y, pi.X, pi.Y )
			edge = Autopilot.graphEdge(pi.Name, gi1.Name)
			Autopilot.SpatialDistances[edge] = distfrom( pi.X, pi.Y, gi1.X, gi1.Y )
	
			edge = Autopilot.graphEdge("player", pi.Name)
			Autopilot.SpatialDistances[edge] = distfrom( playerX, playerY, pi.X, pi.Y )
			edge = Autopilot.graphEdge(pi.Name, "player")
			Autopilot.SpatialDistances[edge] = distfrom( pi.X, pi.Y, playerX, playerY )
		end

		-- include this gate in the list of objects
		table.insert(Autopilot.Objects, gi1.Name)
		Autopilot.numObjects = Autopilot.numObjects + 1
	end

	-- once the gate loop is finished, run through the planets
	-- one time and insert them into the list of objects
	for num,planet in pairs(thePlanets) do
		local pi = Epiar.getPlanetInfo(planet:GetID())
		table.insert(Autopilot.Objects, pi.Name)
		Autopilot.numObjects = Autopilot.numObjects + 1
	end

	-- include the player too
	table.insert(Autopilot.Objects, "player")
	Autopilot.numObjects = Autopilot.numObjects + 1
end

function APCompute (dest)
	if Autopilot.ConfigDialog ~= nil then
		Autopilot.ConfigDialog:close()
		Autopilot.ConfigDialog = nil
		Autopilot.ConfigDest = nil
		Epiar.unpause()
	end

	Autopilot.showGateRoute()

	Autopilot.Objects = { }
	Autopilot.SpatialDistances = { }
	Autopilot.GateRoute = { }

	-- This has to be done each time, but only because the player's position changes;
	-- SpatialDistances could be made persistent, and this could be optimized a bit
	-- (only recompute player-Gate and player-Planet distances).
	calculateSpatialDistances() 

	local exists = function(obj)
		for n,o in pairs(Autopilot.Objects) do
			if o == obj then return true end
		end
		return false
	end

	if dest == nil or dest == "" or exists(dest) == false then
		HUD.newAlert("Please specify a real destination.")
		return
	end

	shortestPath("player", dest)
	HUD.newAlert( string.format("Computed a route to %s: %d gate pair(s).", dest, math.floor(#Autopilot.GateRoute/2) ) )
end

function playerGateAutoAngle ()
	if Autopilot == nil then return end
	if Autopilot.hasAutopilot(PLAYER) == false then
		return
	end

	local theObj = Autopilot.GateRoute[1]
	if theObj == "player" then
		-- this shouldn't happen
	elseif #Autopilot.GateRoute == 0 then
		-- neither should this
	elseif #Autopilot.GateRoute == 1 then
		local pi = Epiar.getPlanetInfo(theObj)
		local playerX, playerY = PLAYER:GetPosition()
		local speed = PLAYER:GetMomentumSpeed()
		-- Work on slowing down if we're getting close to the destination
		if distfrom(playerX, playerY, pi.X, pi.Y) < 800 + 100*speed then
			local inverseMomentumDir = - PLAYER:directionTowards( PLAYER:GetMomentumAngle() )
			if speed > 3 then
				if math.abs( PLAYER:directionTowards( PLAYER:GetMomentumAngle() ) ) > 176 then
					Autopilot.AllowAccel = true
				else
					Autopilot.AllowAccel = false
					PLAYER:Rotate( inverseMomentumDir )
				end
			else
				-- going slow enough already; don't do anything
				Autopilot.AllowAccel = false
			end
		-- Otherwise just keep going in the direction of the destination
		else
			PLAYER:Rotate( PLAYER:directionTowards( pi.X, pi.Y ) )
			if PLAYER:directionTowards( pi.X, pi.Y ) == 0 then
				Autopilot.AllowAccel = true
			else
				Autopilot.AllowAccel = false
			end
		end
		if distfrom(playerX, playerY, pi.X, pi.Y) < 300 then
			table.remove(Autopilot.GateRoute, 1)
			HUD.newAlert("You have arrived at your destination.")
			Autopilot = nil
		end
	else
		local gi = Epiar.getGateInfo(theObj)
		PLAYER:Rotate( PLAYER:directionTowards(gi.X, gi.Y) )

		local playerX, playerY = PLAYER:GetPosition()
		if distfrom(playerX, playerY, gi.X, gi.Y) < 500 then
			-- It's important to be calling this function as you are getting close to the gate so
			-- the route can be updated before you go through. Also, if the route gets updated but
			-- you miss the gate, you need to manually enter the gate and then resume auto-angling
			-- as soon as you exit. (Better would be for the Autopilot state to be updated upon
			-- successful gate travel.)
			table.remove(Autopilot.GateRoute, 1)
			Autopilot.showGateRoute()
			if #Autopilot.GateRoute % 2 == 1 then
				-- don't tell the player to resume acceleration until he/she has cleared both the gate top and bottom
				-- (this acceleration control could be automated)
				--HUD.newAlert("please resume acceleration")
				Autopilot.showAlert()
				Autopilot.AllowAccel = true
			end
		elseif distfrom(playerX, playerY, gi.X, gi.Y) < 800 then
			if Autopilot.AllowAccel ~= false then
				--HUD.newAlert("please stop acceleration")
				Autopilot.AllowAccel = false
			end
		end
	end
end

function playerAutopilotRun ()
	if Autopilot == nil then
		PLAYER:SetLuaControlFunc("")
		return
	end
	if Autopilot.AllowAccel ~= false then
		PLAYER:Accelerate()
	end
	playerGateAutoAngle()
end

function playerAutopilotToggle ()
	if Autopilot == nil then return end
	if Autopilot.Control ~= true then
		Autopilot.showAlert()
		Autopilot.Control = true
		Autopilot.AllowAccel = true
		PLAYER:SetLuaControlFunc("playerAutopilotRun()")
	else
		HUD.newAlert("Autopilot disengaged")
		Autopilot.Control = false
		Autopilot.AllowAccel = false
		PLAYER:SetLuaControlFunc("")
	end
end

function showAPConfigDialog()
	if Autopilot == nil then APInit() end
	if Autopilot.hasAutopilot(PLAYER) == false then
		HUD.newAlert("You don't have an autopilot system.")
		return
	end
	if Autopilot.ConfigDialog ~= nil then return end

	Epiar.pause()

	Autopilot.ConfigDialog = UI.newWindow(400,250,300,150, "Configure autopilot")
	Autopilot.ConfigDest = UI.newTextbox(30,30,200,1, "Ves")
	local APConfigCompute = UI.newButton(30,60, 150, 30, "Compute gate route", "APCompute( Autopilot.ConfigDest:GetText() )" )
	Autopilot.ConfigDialog:add(Autopilot.ConfigDest, APConfigCompute)
end
