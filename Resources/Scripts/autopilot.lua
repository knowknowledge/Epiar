--
-- Gate navigation autopilot for Epiar ships
--   Author: Rikus Goodell
--   Last updated: Dec 7, 2010
--
--   This is an automatic gate navigation system for ships in Epiar. It currently works just fine
--   for the player, but it has not been tested with AIs. Most likely there is still quite a bit
--   of work to be done in that area.
--
--   To-do: (as of Dec 7)
--
--     - Increase OO-ishness and make generic enough for AIs to use (finished?)
--
--     - Use coroutines for calculateSpatialDistances() and/or shortestPath() to smooth it
--       out across multiple ticks rather than hogging the processor (especially important if
--       AIs will be using it). This may be a bit tricky, since they access and edit some
--       shared data, but not as bad as threading.
--
--     - Modularize / make autoAngle() mimic a state machine
--
--     - Update existing AIs to (sometimes) take advantage of this system
--
-- How to use it:
--   0) buy the "Nav Aid" outfit (it won't work until you do)
--   1) hit 'T' (capital) and choose your destination (must be a valid planet/station name)
--   2) press Left Alt
--   3) either wait until you arrive at your destination or hit Left Alt again to deactivate
--

APPersistent = nil	-- this table is non-volatile but is safe to reset if needed
APFuncs = { }		-- this table should never be reset
Autopilot = nil		-- this is the player's autopilot (volatile)

-- Initialize/reset all persistent autopilot data. This should only need to get called one time,
-- but you're allowed to call it again if you really want to! (For example, if the universe changes.)
function APHardInit()
	APPersistent = { }
	APPersistent.Objects = { }
	APPersistent.numObjectsVal = 0
	APPersistent.numObjects = function() return APPersistent.numObjectsVal end
	APPersistent.numObjectsIncr = function() APPersistent.numObjectsVal = APPersistent.numObjectsVal + 1 end
	APPersistent.SpatialDistStatic = { }
	APPersistent.gateInfoCache = { }
	APPersistent.planetInfoCache = { }
end

-- Constructor-like routine for any autopilot object (table must be grabbed from the return value now).
function APInit( _name, _id )
	if APPersistent == nil then APHardInit() end
	if _name == nil or _id == nil then
		print "Bad APInit() call - expected two arguments: name, id"
		return nil
	end
	local self = {
		numObjects	= APFuncs.numObjects,
		spatialDistance	= APFuncs.spatialDistance, 
		showGateRoute	= APFuncs.showGateRoute, 
		graphEdge	= APFuncs.graphEdge, 
		hasAutopilot	= APFuncs.hasAutopilot, 
		showAlert	= APFuncs.showAlert,
		shortestPath	= APFuncs.shortestPath,
		calculateSpatialDistances
				= APFuncs.calculateSpatialDistances,
		compute		= APFuncs.compute,
		autoAngle	= APFuncs.autoAngle
	}
	self.SpatialDistDynamic = { }
	self.GateRoute = { }
	self.control = false
	self.AllowAccel = false
	self.name = _name
	self.id = _id
	self.spcr = nil -- shortestPath() coroutine
	self.spcrConclude = nil
	return self
end

------------------------
-- Begin OO functions --
------------------------

-- Grab object count from persistent data, but include this instance in the total
APFuncs.numObjects = function(self) return APPersistent.numObjects() + 1 end

-- If the specified edge exists in either the persistent or dynamic spatial distances, return the value
APFuncs.spatialDistance = function(self, edge)
	if APPersistent.SpatialDistStatic[edge] ~= nil then return APPersistent.SpatialDistStatic[edge] end
	if self.SpatialDistDynamic[edge] ~= nil then return self.SpatialDistDynamic[edge] end
	return nil
end

-- Mainly useful for debugging
APFuncs.showGateRoute = function(self)
	print ""
	for num =1,#self.GateRoute do
		print (string.format("   GR %d %s", num, self.GateRoute[num]))
	end
end

-- Return a string to be used as a table key representing an edge in the graph
APFuncs.graphEdge = function(self, u, v)
	if u == nil or v == nil then
		print "APFuncs.graphEdge() received one or both values nil."
		return nil
	end
	return (string.format("%s to %s", u, v))
end

-- Does the specified ship have the necessary outfit? (Note: You must pass the sprite itself)
APFuncs.hasAutopilot = function(self, ship)
	for n,po in pairs( ship:GetOutfits() ) do
		if po == "Nav Aid" then return true end
	end
	return false
end

-- Show a HUD alert when something happens on the way to the destination
APFuncs.showAlert = function(self)
	if self == nil or self.name ~= "player" then return end
	local dest = self.GateRoute[#self.GateRoute]
	local next = self.GateRoute[1]
	HUD.newAlert( (string.format("Autopilot: engaged, en route to %s, next object is %s", dest, next) ) )
end

-- Calculate the shortest path from the current location to the specified destination
function APFuncs.shortestPath(self, dest)

	-- Sorry; these obfuscated-looking variables are to best match the algorithm description from my CS textbook!

	self.d = { }  -- shortest path distances from s; e.g., d["Ves"] is the length of the shortest known route to from s to Ves.
	self.pred = { } -- predecessor list; e.g., pred["Ves"] is the gate (if any -- else origin) that the player exits before reaching Ves
	self.s = (self.name .. self.id) -- starting point (most likely "player")
	self.V = APPersistent.Objects -- names of planets, stations, and gates
	self.S = { }
	self.l = function(edge) return self:spatialDistance(edge) end

	-- relax distances between edges based on newly discovered routes
	self.relax = function ( u, v )
		if u == v then return end
		local e_uv = self:graphEdge(u, v)
		local luv = self.l(e_uv)
		-- if luv is nil, then this may be a planet-planet edge; we don't care about those
		if luv == nil then return end
		-- for this algorithm, d[x] == nil shall be taken to mean d(x) = infinity (unexplored); not to be confused with d(x) = 0
		if self.d[u] == nil then return end
		-- if d(v) is infinite or longer than d(u) + l(u,v), then set d(v) = d(u) = l(u,v), and set predecessor(v) = u
		if self.d[v] == nil or self.d[v] > self.d[u] + luv then
			self.d[v] = self.d[u] + luv
			self.pred[v] = u
		end
	end

	-- initialize with values for starting point
	self.d    [self.s] = 0		-- distance to s is 0
	self.S    [self.s] = true	-- s is the only node that has been explored
	self.pred [self.s] = nil	-- s has no predecessor

	local relaxations = 0
	local cycles = 0

	-- My original plan was to implement Dijkstra's algorithm, but I don't know of a priority queue
	-- structure in Lua, and I seem to have better luck with Bellman-Ford. Running time of O(nm)
	-- for n = objs and m = edges is worse than Dijkstra. Ways the running time could be reduced:
	--     - switch to Dijkstra
	--     - only iterate through Gate-Gate and Gate-Planet pairs; Planet-Planet is ignored anyway by the luv == nil test in relax(), but the loop still happens.
	--     - find a faster way of achieving "choose v from V such that v not in S" (choose v from V - S) rather than just testing for existence in S of each object

	-- Fortunately, this only has to run each time the player chooses to calculate a new route, which is rare.

	self.spcr = coroutine.create(function()
		local spcr_finished = false

		for i =1,self:numObjects()-1 do -- <--- repeat (numObjects - 1) times
			for i1,u in pairs(self.V) do -- <----------\_______________ for each
				for i2,v in pairs(self.V) do -- <--/                edge (u,v)
					if self.S[v] == nil then -- such that v not in S
						if( u ~= v ) then self.relax( u, v ) end
						relaxations = relaxations + 1
					end
				end
				-- since the moving object is no longer listed, need to do this relax individually
				-- note: the fact that we are considering that u (rather than v) may be unexplored deviates
				-- from the algorithm spec a bit, but with the way things are implemented here it does not
				-- cause a problem
				if(self.S[u] == nil) then self.relax( (self.name .. self.id), u) end
				if i1 % 7 == 1 then coroutine.yield() end
			end
			coroutine.yield()
		end

		spcr_finished = true

		return spcr_finished
	end)

	self.spcrConclude = function()
		self.spcr = nil
		if self.name == "player" then PLAYER:SetLuaControlFunc("") end

		--print ("relaxations: "..relaxations)

		if dest == self.s then
			-- this should not happen unless the player asks the for a route to "player"
		else
			self.GateRoute = { }

			self.buildRoute = function(obj)
				table.insert(self.GateRoute, 0, obj)
				if obj == (self.name .. self.id) or obj == nil then return true end
				return self.buildRoute(self.pred[obj])
			end

			self.buildRoute(dest)
		end
		--self:showGateRoute()

		if self.name == "player" then
			HUD.newAlert( string.format("Computed a route to %s: %d gate pair(s).", dest, math.floor(#self.GateRoute/2) ) )
		end

		return true
	end

	self.spcrTick = function()
		--print "SPCR TICK"
		coroutine.resume(self.spcr)
		local status, spcr_finished = coroutine.resume(self.spcr)
		if spcr_finished then
			--print "coroutine finished"
			return self.spcrConclude()
		else
			--print "coroutine did not finish yet"
			return false
		end
	end

	if self.name == "player" then
		PLAYER:SetLuaControlFunc("Autopilot.spcrTick()")
		HUD.newAlert("Working...")
	end
end

function APFuncs.calculateSpatialDistances (self, moving_type, moving_id)
	local movingX, movingY, moving_object
	if moving_type == "player" then
		movingX, movingY = PLAYER:GetPosition()
	elseif moving_type == "AI" then
		movingX, movingY = Epiar.getSprite(moving_id):GetPosition()
	end
	moving_object = (moving_type .. moving_id)

	local updateOnly = true
	if self:numObjects() == 1 then updateOnly = false end

	if APPersistent.doneCaching ~= true then
		local theGates = Epiar.gates()
		local thePlanets = Epiar.planets()
		for num,gate in pairs(theGates) do
			local gi = Epiar.getGateInfo(gate:GetID())
			APPersistent.gateInfoCache[gi.Name] = gi
		end
		for num,planet in pairs(thePlanets) do
			local pi = Epiar.getPlanetInfo(planet:GetID())
			APPersistent.planetInfoCache[pi.Name] = pi
		end
		APPersistent.doneCaching = true
	end

	for gname1,gi1 in pairs(APPersistent.gateInfoCache) do

		-- loop to calculate spatial distances between all gate pairs (including partners, which are recorded as zero distance)
		for gname2,gi2 in pairs(APPersistent.gateInfoCache) do
			if updateOnly then break end
			local edge = self:graphEdge(gi1.Name, gi2.Name)
			APPersistent.SpatialDistStatic[edge] = distfrom( gi1.X, gi1.Y, gi2.X, gi2.Y )
			if gi1.Exit == gi2.Name then 
				APPersistent.SpatialDistStatic[edge] = 0
			end
		end

		-- calculate distance from moving object to this gate
		local edge = self:graphEdge(moving_object, gi1.Name)
		self.SpatialDistDynamic[edge] = distfrom( movingX, movingY, gi1.X, gi1.Y )
		edge = self:graphEdge(gi1.Name, moving_object)
		self.SpatialDistDynamic[edge] = distfrom( gi1.X, gi1.Y, movingX, movingY )

		-- loop to calculate distances Gate-Planet and moving_object-Planet
		for pname,pi in pairs(APPersistent.planetInfoCache) do
			if updateOnly == false then
				local edge = self:graphEdge(gi1.Name, pi.Name)
				APPersistent.SpatialDistStatic[edge] = distfrom( gi1.X, gi1.Y, pi.X, pi.Y )
				edge = self:graphEdge(pi.Name, gi1.Name)
				APPersistent.SpatialDistStatic[edge] = distfrom( pi.X, pi.Y, gi1.X, gi1.Y )
			end
	
			edge = self:graphEdge(moving_object, pi.Name)
			self.SpatialDistDynamic[edge] = distfrom( movingX, movingY, pi.X, pi.Y )
			edge = self:graphEdge(pi.Name, moving_object)
			self.SpatialDistDynamic[edge] = distfrom( pi.X, pi.Y, movingX, movingY )
		end

		if updateOnly == false then
			-- include this gate in the list of objects
			table.insert(APPersistent.Objects, gi1.Name)
			APPersistent.numObjectsIncr()
		end
	end

	-- once the gate loop is finished, run through the planets
	-- one time and insert them into the list of objects
	for pname,pi in pairs(APPersistent.planetInfoCache) do
		if updateOnly then break end
		table.insert(APPersistent.Objects, pi.Name)
		APPersistent.numObjectsIncr()
	end

	-- don't do this anymore
	--if APPersistent.Objects[moving_object] == nil then
	--	table.insert(APPersistent.Objects, moving_object)
	--	APPersistent.numObjectsIncr()
	--end

end

function APFuncs.compute (self, dest)
	if self.ConfigDialog ~= nil then
		self.ConfigDialog:close()
		self.ConfigDialog = nil
		self.ConfigDest = nil
		Epiar.unpause()
	end

	-- This has to be done each time, but only because the ship's position changes.
	-- Distances between stationary objects are computed once and then kept for
	-- future use (shared between instances). However, different instances need to
	-- have their own distance data isolated from one another to avoid clogging
	-- up other computations.

	self:calculateSpatialDistances(self.name, self.id) 

	local exists = function(obj)
		for n,o in pairs(APPersistent.Objects) do
			if o == obj then return true end
		end
		return false
	end

	if dest == nil or dest == "" or exists(dest) == false then
		if self.name == "player" then
			HUD.newAlert("Please specify a real destination.")
		end
		return
	end

	self:shortestPath(dest)
end

-- Function to handle ship rotation and thrust suggestions. Returns true until arrival at the destination. then false.
function APFuncs.autoAngle (self)
	if self == nil then return end
	-- if it's not an AI, check the outfit list to make sure this ship actually has an autopilot installed
	-- (currently, AIs are allowed free use of autopilot functions so you don't have to worry about installing outfits on AIs)
	local mySprite = Epiar.getSprite(self.id)
	if self.name ~= "AI" and self:hasAutopilot(mySprite) == false then return end

	self.AllowAccel = false

	local theObj = self.GateRoute[1]
	if theObj == (self.name .. self.id) then
		-- this shouldn't happen
	elseif #self.GateRoute == 0 then
		-- neither should this
	elseif #self.GateRoute == 1 then
		local pi = APPersistent.planetInfoCache[theObj]
		local movingX, movingY = mySprite:GetPosition()
		local speed = mySprite:GetMomentumSpeed()
		self.AllowAccel = false
		-- Work on slowing down if we're getting close to the destination
		if distfrom(movingX, movingY, pi.X, pi.Y) < 800 + 100*speed then
			local inverseMomentumDir = - mySprite:directionTowards( mySprite:GetMomentumAngle() )
			if speed > 3 then
				if math.abs( mySprite:directionTowards( mySprite:GetMomentumAngle() ) ) > 176 then
					self.AllowAccel = true
				else
					self.AllowAccel = false
					mySprite:Rotate( inverseMomentumDir )
				end
			else
				-- going slow enough already; don't do anything
				self.AllowAccel = false
			end
		-- Otherwise just keep going in the direction of the destination
		else
			mySprite:Rotate( mySprite:directionTowards( pi.X, pi.Y ) )
			self.AllowAccel = (mySprite:directionTowards( pi.X, pi.Y ) == 0)
		end
		if distfrom(movingX, movingY, pi.X, pi.Y) < 300 then
			table.remove(self.GateRoute, 1)
			if self.name == "player" then
				HUD.newAlert("You have arrived at your destination.")
				Autopilot = nil
			end
			self = nil
			return false
		end
	else
		local gi = APPersistent.gateInfoCache[theObj]
		mySprite:Rotate( mySprite:directionTowards(gi.X, gi.Y) )
		self.AllowAccel = false

		local movingX, movingY = mySprite:GetPosition()
		if distfrom(movingX, movingY, gi.X, gi.Y) < 500 then
			-- It's important to be calling this function as you are getting close to the gate so
			-- the route can be updated before you go through. Also, if the route gets updated but
			-- you miss the gate, you need to manually enter the gate and then resume auto-angling
			-- as soon as you exit. (Better would be for the self state to be updated upon
			-- successful gate travel.)
			table.remove(self.GateRoute, 1)
			theObj = self.GateRoute[1]
			local oi = APPersistent.gateInfoCache[theObj]
			if oi == nil then oi = APPersistent.planetInfoCache[theObj] end
			--self:showGateRoute()
			if #self.GateRoute % 2 == 1 then
				-- don't tell the moving to resume acceleration until he/she has cleared both the gate top and bottom
				-- (this acceleration control could be automated)
				--HUD.newAlert("please resume acceleration")
				self.AllowAccel = (mySprite:directionTowards( oi.X, oi.Y ) == 0)
				self:showAlert()
			end
		elseif distfrom(movingX, movingY, gi.X, gi.Y) < 800 then
			if self.AllowAccel ~= false then
				--HUD.newAlert("please stop acceleration")
				self.AllowAccel = false
			end
		else
			self.AllowAccel = (#self.GateRoute % 2 == 1 and mySprite:directionTowards( gi.X, gi.Y ) == 0)
		end
	end
	return true
end

------------------------------------------
-- Begin non-OO functions (player only) --
------------------------------------------

function playerAutopilotRun ()
	if Autopilot == nil then
		PLAYER:SetLuaControlFunc("")
		return
	end
	if Autopilot.AllowAccel ~= false then
		PLAYER:Accelerate()
	end
	Autopilot:autoAngle()
end

function playerAutopilotToggle ()
	if Autopilot == nil then return end
	if Autopilot.control ~= true then
		Autopilot:showAlert()
		Autopilot.control = true
		Autopilot.AllowAccel = true
		PLAYER:SetLuaControlFunc("playerAutopilotRun()")
		Fleets:getShipFleet(PLAYER:GetID()):gateTravel(nil, Autopilot.GateRoute)
	else
		HUD.newAlert("Autopilot disengaged")
		Autopilot.control = false
		Autopilot.AllowAccel = false
		PLAYER:SetLuaControlFunc("")
		Fleets:getShipFleet(PLAYER:GetID()):formation()
	end
end

function showAPConfigDialog()
	-- The "Autopilot" global refers to the player's autopilot, if any. Any others will be stored elsewhere.
	if Autopilot == nil then Autopilot = APInit( "player", PLAYER:GetID() ) end
	if Autopilot:hasAutopilot(PLAYER) == false then
		HUD.newAlert("You don't have an autopilot system.")
		Autopilot = nil
		return
	end
	if Autopilot.ConfigDialog ~= nil then return end

	-- if it's already in control, turn it off for a moment (helps avoid having to hit the same key twice)
	if Autopilot.control == true then
		playerAutopilotToggle()
	end

	Epiar.pause()

	local width = 250
	local height =  85 + (16*#Epiar.planetNames())

	Autopilot.ConfigDialog = UI.newWindow(400,175,height,width, "Configure autopilot")
	local frame = UI.newFrame(10,30,230,110)
	local destLabel = UI.newLabel(15,10, "select destination:")
	-- +10 and +30 to match offsets for the frame of which this is not actually a child
	Autopilot.ConfigDest = UI.newDropdown(15+10,35+30,200,16)
	for n,p in pairs( Epiar.planetNames() ) do
		Autopilot.ConfigDest:addOption(p)
	end
	Autopilot.ConfigDest:setText( "Ves" )
	local APConfigCompute = UI.newButton(15,60, 200, 30, "Compute gate route", "Autopilot:compute( Autopilot.ConfigDest:GetText() )" )
	-- This instruction text may seem superfluous, but it does serve the purpose
	-- of occupying the extra space needed to accommodate the dropdown.
	local instructionsLabel = UI.newParagraph(20, 160, width, height, 
[[Select a destination from the menu, compute the gate route (takes several seconds), then
hit Left Alt to engage or disengage the autopilot. The route will be shared with any escorts,
who will continue to accompany you.

You should keep the autopilot engaged until you reach your destination. In the case of
some malfunction (e.g. missing a gate), you may need to recompute your route.]]
	)
	frame:add(destLabel, APConfigCompute)
	-- note: add dropdown to the window but on top of the frame so it is not clipped
	Autopilot.ConfigDialog:add(instructionsLabel, frame, Autopilot.ConfigDest)
	Autopilot.ConfigDialog:addCloseButton()
end
