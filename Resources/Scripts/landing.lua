
--- Land on a planet
function landingDialog(id)
	local planet = Epiar.getSprite(id)
	-- Create the Planet Landing Screen
	if UI.search( string.format("/Window'%s'/Tabs'Store'/", planet:GetName())) ~= nil then return end

	Epiar.pause()
	
	local height = 500
	local width = 600
	local boxsize = 120

	landingWin = UI.newWindow( 200,100,width,height, string.format("%s",planet:GetName()))
	storeframe = UI.newTabCont( 10, 30, width - 20, height - 80,"Store")
	landingWin:add(storeframe)

	function addToStoreList( storeList, list, yoff, cmd, container )
		for i,name in ipairs(list) do
			local callback = string.format( cmd, container, name )
			local pic = UI.newPicture( 15, yoff, boxsize, boxsize, name, 0, 0, 0, 1)
			pic:setLuaClickCallback( callback )
			storeList:add( pic )
			storeList:add( UI.newButton( 15, yoff+boxsize, boxsize, 20, name, callback ))
			yoff = yoff + 30 + boxsize
		end
		return yoff
	end

	-- Shipyard
	local shipyard = UI.newTab("ShipYard")
	local shipList = UI.newFrame( 10, 10, 160, 360 )
	shipyard:add( shipList )
	local yoff = 10
	local models = planet:GetModels()
	local shipyardPath = string.format("/Window'%s'/Tabs'Store'/'ShipYard'/", planet:GetName() )
	yoff = addToStoreList( shipList, models, yoff, "storeView(%q, 'ship', %q)", shipyardPath )
	shipyard:add( UI.newButton( width-150,340,100,30,"Buy","buyShip()" ))
	storeframe:add(shipyard)

	if #models > 0 then
		storeView( shipyardPath, 'ship',models[1])
	end

	-- Outfitting
	outfitting = UI.newTab("Outfitting")
	local outfitList = UI.newFrame( 10, 10, 160, 360 )
	outfitting:add( outfitList )
	yoff = 10
	local weapons = planet:GetWeapons()
	local engines = planet:GetEngines()
	local outfits = planet:GetOutfits()
	local outfitPath = string.format( "/Window'%s'/Tabs'Store'/'Outfitting'/", planet:GetName() )
	yoff = addToStoreList( outfitList, weapons, yoff, "storeView(%q, 'weapon', %q)", outfitPath)
	yoff = addToStoreList( outfitList, engines, yoff, "storeView(%q, 'engine', %q)", outfitPath)
	yoff = addToStoreList( outfitList, outfits, yoff, "storeView(%q, 'outfit', %q)", outfitPath)
	storeframe:add(outfitting)
	outfitting:add( UI.newButton( width-250,340,100,30,"Sell","sellOutfit()" ))
	outfitting:add( UI.newButton( width-150,340,100,30,"Buy","buyOutfit()" ))

	if #weapons > 0 then     storeView( outfitPath, 'weapon', weapons[1])
	elseif #engines > 0 then storeView( outfitPath, 'engine', engines[1])
	elseif #outfits > 0 then storeView( outfitPath, 'outfit', outfits[1])
	end

	-- Trade
	trade = UI.newTab("Trade")
	tradeCounts = {} -- This global variable
	local commodities = Epiar.commodities()
	local currentCargo,stored,storable = PLAYER:GetCargo()
	for i,commodity in pairs(commodities) do
		local yoff = 20+i*20
		local msrp = Epiar.getMSRP(commodity)
		local price_offset = math.random(-3,3)
		local priceMeanings = { "(Very Low)","(Low)","","","","(High)","(Very High)" }
		local price = msrp + ( price_offset*msrp/10 )
		--print (commodity.."is "..priceMeanings[price_offset+4].." at "..price.." instead of "..msrp)
		local count = 10
		trade:add( UI.newLabel(10,yoff,string.format("%s at %d %s",commodity,price,priceMeanings[price_offset+4]),0) )
		tradeCounts[commodity] = UI.newTextbox(300,yoff,30,1, currentCargo[commodity] or 0, commodity)
		trade:add( tradeCounts[commodity] )
		trade:add( UI.newButton(330,yoff,30,20,"Buy",string.format("tradeCommodity('buy',%q,%d,%d)",commodity,count,price )))
		trade:add( UI.newButton(360,yoff,30,20,"Sell",string.format("tradeCommodity('sell',%q,%d,%d)",commodity,count,price )))
	end
	storeframe:add(trade)

	-- Employment
	missions = UI.newTab("Employment")
	availableMissionsTypes = {"ReturnAmbassador", "DestroyPirate", "CollectArtifacts", "ShippingRoutes", "ProtectFreighter" }
	rareMissionTypes = {"DestroyGaryTheGold"}
	availableMissions = {} -- This is a global variable
	yoff = 5
	local numMissions = math.random(2,7)
	function accept( missionType, i )
		PLAYER:AcceptMission(missionType, availableMissions[i])

		local path = string.format("/Window'%s'/Tabs/Tab'Employment'/Frame[%d]/Button/", planet:GetName(), i-1)
		local acceptButton = UI.search( path )
		if acceptButton ~= nil then
			acceptButton:close()
		end
	end
	for i = 1,numMissions do
		local missionType
		if math.random(50) == 1 then
			missionType = choose(rareMissionTypes)
		else
			missionType = choose(availableMissionsTypes)
		end
		availableMissions[i] = _G[missionType].Create()
		missions:add(
			UI.newFrame( 10, yoff, width -70, 150,
				UI.newLabel( 10, 10, availableMissions[i].Name ),
				UI.newLabel( 10, 40, linewrap(availableMissions[i].Description) ),
				UI.newButton( width-190, 20, 100, 20, "Accept",  string.format("accept(%q, %d)", missionType, i) )
			)
		)
		yoff = yoff + 170
	end
	storeframe:add(missions)

	landingWin:add(UI.newButton( 10,height-40,100,30,"Repair","PLAYER:Repair(10000)" ))
	landingWin:add(UI.newButton( 110,height-40,100,30,"Weapon Config","weaponConfigDialog()" ))
	landingWin:add(UI.newButton( width-110,height-40,100,30,string.format("Leave "), "Epiar.savePlayer();Epiar.unpause();landingWin:close();landingWin=nil" ))
end

