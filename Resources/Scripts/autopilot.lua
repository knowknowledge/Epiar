-- Autopilot for Epiar player
--
--    A WORK IN PROGRESS
--

-- some revision history is in RCS
--   $Id: autopilot.lua,v 1.10 2010/11/28 23:22:13 rikus Exp $

-- Nov 28
--   I'm not sure if this will be useful or not, but I decided to work on it.
--   This is still in the very early stages and quite messy, but I have a partially automatic
--   gate navigation system that works. Once this is cleaned up a bit, it might make sense to
--   turn it into an AI and allow the player to be controlled by that AI.
--   Example : one state would be for "coasting into the gate once proper momentum angle is achieved").
--   Then it could also guide escorts, for example, through the gates with you (however, a case like that
--   might make it tempting for the autopilot to recompute shortest path every time the player moves,
--   which would be unacceptably CPU intensive, so a better way of tracking the player would need to be
--   devised.
--
--   This should probably also be made into something that you get after buying an outfit or with certain
--   ships.
--
-- How to use it:
--   1) hit 'T' (capital) and choose your destination (must be a valid planet/station name)
--   2) hold down Left Alt and accelerate in the direction it takes you.
--   3) if you think you are headed completely in the right direction or if it tells you to stop,
--      stop accelerating, but keep Left Alt held down. (this could be automated)
--   4) when you are through the gate and it tells you to resume acceleration, start accelerating
--      again with Left Alt still held.
--   5) repeat until you arrive at your destination

spatialDistance = distfrom

Autopilot = { }

Autopilot.SpatialDistances = { } 
Autopilot.Objects = { }

Autopilot.numObjects = 0

function shortestPath(orig, dest)

	local d = { }  -- path distances from s
	local pred = { } -- predecessor list
	local s = orig -- starting point
	local V = Autopilot.Objects

	local S = { }
	local d = { }
	local l = Autopilot.SpatialDistances

	function relax( u, v, luv )
		local e_uv = (string.format("%s to %s", u, v))
		local e_su = (string.format("%s to %s", s, u))
		if u == v then return end
		if d[u] == nil then return end
		if d[v] == nil or d[v] > d[u] + luv then
		--if d[v] == nil or d[v] > d[u] + luv then
			d[v] = d[u] + luv
			pred[v] = u
		end
	end

	-- initialize with values for starting point
	d[s] = 0
	S[s] = true
	pred[s] = nil

	local iterations = 0

	-- My original plan was to implement Dijkstra's algorithm, but I don't know of a priority queue
	-- structure in Lua, and I seem to have better luck with Bellman-Ford. Running time of O(nm)
	-- for n = objs and m = edges is worse than Dijkstra. Ways the running time could be reduced:
	--     - switch to Dijkstra
	--     - only iterate through Gate-Gate and Gate-Planet pairs (Planet-Planet is ignored anyway by the luv ~= nil test but the loop still happens)
	--     - find a faster way of achieving "choose v from V such that v not in S" (choose v from V - S) rather than just testing for existence in S of each object

	-- Fortunately, this only has to run each time the player chooses to calculate a new route, which is rare.

	for i= 1,Autopilot.numObjects-1 do -- <--- repeat (Autopilot.numObjects - 1) times
		for i1,u in pairs(V) do -- <----------\_______________ for each
			for i2,v in pairs(V) do -- <--/                edge (u,v)

				if S[v] == nil then -- such that v not in S
					local luv = l[u.." to "..v]
					if luv ~= nil then -- if spatial distance is pre-calculated (if not, then we don't care)
						if( u ~= v ) then relax( u, v, luv ) end
						iterations = iterations + 1
					end
				end

			end
		end
	end

	print ("iterations: "..iterations)

	print (string.format("distance %s to %s: %f", orig, dest, d[dest]))

	--for i1,dest in pairs(V) do
		-- show all the path calculations
		print ("dest is "..dest)
		table.insert(Autopilot.GateRoute, 0, dest)
		print ("  d[dest] is "..d[dest])
		if dest == orig then
			print "dest is same as orig"
		else

			local p = pred[dest]
			print ("the pred is "..p)
			while p ~= orig do
				print ("pred: "..p.." dist from s:"..(d[p]))
				table.insert(Autopilot.GateRoute, 0, p)
				print (string.format("spatial distance p:%s to pred[p]:%s is %f", p, pred[p], l[p.." to "..pred[p]]))
				p = pred[p]
			end
			print (orig)
			table.insert(Autopilot.GateRoute, 0, orig)
			print ""
			print ""
		end
	--end
end

Autopilot.GateRoute = { }

function calculateSpatialDistances ()
	local theGates = Epiar.gates()
	local thePlanets = Epiar.planets()
	local playerX, playerY = PLAYER:GetPosition()
	Autopilot.numObjects = 0
	--theGateNames = Epiar.gateNames()
	for num,gate1 in pairs(theGates) do
		local gi1 = Epiar.getGateInfo(gate1:GetID())
		for num,gate2 in pairs(theGates) do
			local gi2 = Epiar.getGateInfo(gate2:GetID())
			--print (string.format("Name: %s  X: %d  Y: %d  Exit: %s\n", gateInfo.Name, gateInfo.X, gateInfo.Y, gateInfo.Exit))
			local edge = string.format("%s to %s", gi1.Name, gi2.Name)
			Autopilot.SpatialDistances[edge] = spatialDistance( gi1.X, gi1.Y, gi2.X, gi2.Y )
			if gi1.Exit == gi2.Name then 
				Autopilot.SpatialDistances[edge] = 0
			end
		end
		local edge = string.format("player to %s", gi1.Name)
		Autopilot.SpatialDistances[edge] = spatialDistance( playerX, playerY, gi1.X, gi1.Y )
		edge = string.format("%s to player", gi1.Name)
		Autopilot.SpatialDistances[edge] = spatialDistance( gi1.X, gi1.Y, playerX, playerY )
		for num,planet in pairs(thePlanets) do
			local pi = Epiar.getPlanetInfo(planet:GetID())
			--print (string.format("planet Name: %s  X: %d  Y: %d\n", pi.Name, pi.X, pi.Y))
			local edge = string.format("%s to %s", gi1.Name, pi.Name)
			Autopilot.SpatialDistances[edge] = spatialDistance( gi1.X, gi1.Y, pi.X, pi.Y )
			edge = string.format("%s to %s", pi.Name, gi1.Name) -- flip it in case it's searched in that order
			Autopilot.SpatialDistances[edge] = spatialDistance( gi1.X, gi1.Y, pi.X, pi.Y )
	
			edge = string.format("player to %s", pi.Name)
			Autopilot.SpatialDistances[edge] = spatialDistance( playerX, playerY, pi.X, pi.Y )
			edge = string.format("%s to player", pi.Name)
			Autopilot.SpatialDistances[edge] = spatialDistance( pi.X, pi.Y, playerX, playerY )
		end
		table.insert(Autopilot.Objects, gi1.Name)
		Autopilot.numObjects = Autopilot.numObjects + 1
	end
	for num,planet in pairs(thePlanets) do
		local pi = Epiar.getPlanetInfo(planet:GetID())
		table.insert(Autopilot.Objects, pi.Name)
		Autopilot.numObjects = Autopilot.numObjects + 1
	end
	table.insert(Autopilot.Objects, "player")
end

function testsp (dest)
	if APConfigDialog ~= nil then
		APConfigDialog:close()
		APConfigDialog = nil
		Epiar.unpause()
	end

	print ("testsp got dest "..dest)

	showgateroute()

	Autopilot.Objects = { }
	Autopilot.SpatialDistances = { }
	Autopilot.GateRoute = { }
	calculateSpatialDistances()
	if dest == nil then dest = "Facility 33" end
	shortestPath("player", dest)

end

function playerGateAutoAngle ()
	local theObj = Autopilot.GateRoute[1]
	if theObj == "player" then
	elseif #Autopilot.GateRoute == 0 then
	elseif #Autopilot.GateRoute == 1 then
		local pi = Epiar.getPlanetInfo(theObj)
		local playerX, playerY = PLAYER:GetPosition()
		PLAYER:Rotate( PLAYER:directionTowards( pi.X, pi.Y ) )
		if spatialDistance(playerX, playerY, pi.X, pi.Y) < 300 then
			table.remove(Autopilot.GateRoute, 1)
			HUD.newAlert("You have arrived at your destination.")
		end
	else
		local gi = Epiar.getGateInfo(theObj)
		--print ("rotating toward "..gi.Name.." at "..gi.X..","..gi.Y)
		PLAYER:Rotate( PLAYER:directionTowards(gi.X, gi.Y) )

		local playerX, playerY = PLAYER:GetPosition()
		if spatialDistance(playerX, playerY, gi.X, gi.Y) < 500 then
			table.remove(Autopilot.GateRoute, 1)
			showgateroute()
			if #Autopilot.GateRoute % 2 == 1 then
				-- don't tell the player to resume acceleration until he/she has cleared both the gate top and bottom
				HUD.newAlert("please resume acceleration")
				APAllowAccel = true
			end
		elseif spatialDistance(playerX, playerY, gi.X, gi.Y) < 2500 then
			if APAllowAccel ~= false then
				HUD.newAlert("please stop acceleration")
				APAllowAccel = false
			end
		end
	end
	--showgateroute()
end

function showgateroute()
	print ""
	for num =1,#Autopilot.GateRoute do
		print (string.format("   GR %d %s", num, Autopilot.GateRoute[num]))
	end
end

function showAPConfigDialog()
	if APConfigDialog ~= nil then return end

	Epiar.pause()

	APConfigDialog = UI.newWindow(400,250,300,150, "Configure autopilot")
	APConfigDest = UI.newTextbox(30,30,200,1, "Ves")
	local APConfigCompute = UI.newButton(30,60, 150, 30, "Compute gate route", "testsp( APConfigDest:GetText() )" )
	APConfigDialog:add(APConfigDest, APConfigCompute)
end
