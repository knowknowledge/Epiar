-- height and width of the landing window
width = WIDTH*.6
height = HEIGHT*.6

--- Land on a planet
function landingDialog(id)
	local planet = Epiar.getSprite(id)

	-- Create the Planet Landing Screen
	if UI.search( string.format("/Window'%s'/Tabs'Store'/", planet:GetName())) ~= nil then return end

	Epiar.pause()

	local landingWin = UI.newWindow( WIDTH/2-width/2,HEIGHT/2-height/2,width,height, string.format("%s",planet:GetName()))
	landingTabs = UI.newTabContainer( 10, 30, width - 20, height - 80,"Store")
	landingWin:add(landingTabs)

	function addToStoreList( storeList, list, yoff, cmd, container )
		local boxsize = width/4 - 40
		for i,name in ipairs(list) do
			local callback = string.format( cmd, container, name )
			local pic = UI.newPicture( 0, yoff, boxsize, boxsize, name, 0, 0, 0, 1)
			pic:setPictureCenter( 60, yoff + 50 )
			pic:addCallback( Action_MouseLUp, callback )
			storeList:add( pic )
			storeList:add( UI.newButton( 0, yoff+boxsize, boxsize, 20, name, callback ))
			yoff = yoff + 30 + boxsize
		end
		return yoff
	end

	-- Summary
	local summary = UI.newTab("Summary")
	local summary_frame = UI.newFrame( 10, 10, width - 50, height - 120 )
	local surface_str = planet:GetSurfaceImage()
	local surface = UI.newPicture( (width / 2) - 215, 15, 375, 281, surface_str, 0, 0, 0, 1)
	summary_frame:add( surface )

	summary_frame:add( UI.newParagraph(50, 315, 375, 281, planet:GetSummary() ) )

	summary:add( summary_frame )

	landingTabs:add(summary)

	-- Shipyard
	local shipyard = UI.newTab("Shipyard")
	local shipList = UI.newFrame( 10, 10, width/4 , height - 150)
	shipyard:add( shipList )
	local yoff = 10
	local models = planet:GetModels()
	local shipyardPath = string.format("/Window'%s'/Tabs'Store'/'Shipyard'/", planet:GetName() )
	yoff = addToStoreList( shipList, models, yoff, "storeView(%q, 'ship', %q)", shipyardPath )
	shipyard:add( UI.newButton( width-150, height-135, 100, 30, "Buy", "buyShip()" ))
	landingTabs:add(shipyard)

	if #models > 0 then
		storeView( shipyardPath, 'ship', models[1])
	end

	-- Outfitting
	outfitting = UI.newTab("Outfitting")
	local outfitList = UI.newFrame( 10, 10, width/4 , height - 150)
	outfitting:add( outfitList )
	yoff = 10
	local weapons = Union( planet:GetWeapons(), PLAYER:GetWeapons() )
	local engines = planet:GetEngines()
	local outfits = Union( planet:GetOutfits(), PLAYER:GetOutfits() )
	local outfitPath = string.format( "/Window'%s'/Tabs'Store'/'Outfitting'/", planet:GetName() )
	yoff = addToStoreList( outfitList, weapons, yoff, "storeView(%q, 'weapon', %q)", outfitPath)
	yoff = addToStoreList( outfitList, engines, yoff, "storeView(%q, 'engine', %q)", outfitPath)
	yoff = addToStoreList( outfitList, outfits, yoff, "storeView(%q, 'outfit', %q)", outfitPath)
	landingTabs:add(outfitting)
	outfitting:add( UI.newButton( width-250, height-135, 100, 30, "Sell", "sellOutfit()" ))
	outfitting:add( UI.newButton( width-150, height-135, 100, 30, "Buy", "buyOutfit()" ))

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
	landingTabs:add(trade)

	-- Employment
	missions = UI.newTab("Employment")
	availableMissionsTypes = {"ReturnAmbassador", "DestroyPirate", "CollectArtifacts", "ShippingRoutes", "ProtectFreighter" }
	rareMissionTypes = {"DestroyGaryTheGold"}
	availableMissions = {} -- This is a global variable
	yoff = 5
	local numMissions = math.random(2,7)
	function accept( missionType, i )
		local confirmed = UI.newConfirm("Accept Mission:\n"..availableMissions[i].Name)
		if confirmed then
			PLAYER:AcceptMission(missionType, availableMissions[i])
			local path = string.format("/Window'%s'/Tabs/Tab'Employment'/Frame[%d]/Button/", planet:GetName(), i-1)
			local acceptButton = UI.search( path )
			if acceptButton ~= nil then
				acceptButton:close()
			end
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
		availableMissions[i].Type = missionType
		local fwidth = width-70
		local description = UI.newParagraph( 10, 40, fwidth-130, 150, availableMissions[i].Description )
		local fheight = description:GetH() + 70
		missions:add(
			UI.newFrame( 10, yoff, fwidth, fheight,
				UI.newLabel( fwidth/2, 10, availableMissions[i].Name, 1 ),
				description,
				UI.newButton( fwidth -100 -20, fheight -20 -20, 100, 20, "Accept",  string.format("accept(%q, %d)", missionType, i) )
			)
		)
		yoff = yoff + fheight + 20
	end
	landingTabs:add(missions)

	function Leave()
		Epiar.savePlayer()
		Epiar.unpause()
		local landingWin = UI.search(string.format("/Window'%s'/", planet:GetName()))
		if landingWin then
			landingWin:close()
		end
	end

	landingWin:add(UI.newButton( 10,height-40,100,30,"Repair","PLAYER:Repair(10000)" ))
	landingWin:add(UI.newButton( 110,height-40,100,30,"Weapon Config","weaponConfigDialog()" ))
	landingWin:add(UI.newButton( 210,height-40,100,30,"Information","playerInformation()" ))
	local closeButton = UI.newButton( width-110,height-40,100,30,string.format("Depart"), "Leave()" )
	landingWin:add( closeButton )
	landingWin:setFormButton( closeButton )
	landingWin:addCallback( Action_Close, 'Leave()' )
end

--- Buys a ship
function buyShip(model)
	if model==nil then
		local viewerLabel = UI.search("/Window/Tabs'Store'/'Shipyard'/Frame[1]/Label[0]/")
		if viewerLabel ~= nil then
			model = viewerLabel:GetText()
			--print('Buying ',model)
		else
			print('Error: Could not interpret nil model for buyShip()')
			return
		end
	end
	--print('Buying ',model)
	local price = Epiar.getMSRP(model)
	local player_credits = PLAYER:GetCredits()
	if player_credits >= price then
		currentModel = PLAYER:GetModelName()
		if currentModel ~= model then
			PLAYER:SetCredits( player_credits - price + (2/3.0)*(Epiar.getMSRP(currentModel)) )
			UI.newAlert("Enjoy your new "..model.." for "..price.." credits.")

			-- clear these based on the old slot list
			for slot,weap in pairs( PLAYER:GetWeaponSlotContents() ) do
				PLAYER:RemoveFromWeaponList(weap)
				HUD.closeStatus(weap..":");
			end

			PLAYER:SetModel(model) -- slot list gets updated by SetModel()

			-- update weapon list and HUD to match the new slot list
			for slot,weap in pairs( PLAYER:GetWeaponSlotContents() ) do
				PLAYER:AddToWeaponList(weap)
				HUD.newStatus(weap..":", 130, UPPER_LEFT, string.format("playerAmmo(%q)",weap))
			end

			PLAYER:Repair(10000)
		else
			UI.newAlert("You already have a "..model)
		end
	else
		UI.newAlert("You can't afford to buy a "..model.."\n"..
			string.format("You only have %d credits, but you need %d.",player_credits,price))
	end
	return 1
end

function buyOutfit(outfit)
	if outfit==nil then
		local viewerLabel = UI.search("/Window/Tabs'Store'/'Outfitting'/Frame[1]/Label[0]/")
		if viewerLabel ~= nil then
			outfit = viewerLabel:GetText()
			--print('Buying ',outfit)
		else
			print('Error: Could not interpret nil model for buyShip()')
			return
		end
	end
	local price = Epiar.getMSRP(outfit)
	local player_credits = PLAYER:GetCredits()
	if player_credits < price then
		--print("Account overdrawn...")
		UI.newAlert("You can't afford to buy a "..outfit)
		return
	end

	--print("Debiting your account...")


	--print("Installing Outfit...")
	
	if ( Set(Epiar.weapons())[outfit] ) then
		--print("Weapon...")

		local weapCount = 0;
		for slot,weap in pairs( PLAYER:GetWeaponSlotContents() ) do
			if weap ~= "" then
				weapCount = weapCount + 1
			end
		end

		local weaponInfo = Epiar.getWeaponInfo(outfit)
		if weaponInfo["Ammo Consumption"] ~= 0 then
			PLAYER:AddAmmo(outfit,100)
			UI.newAlert( (string.format("Added 100 ammo for %s",outfit ) ) )
		end

		local wsCount = PLAYER:GetWeaponSlotCount();
		if weapCount >= wsCount then
			UI.newAlert( "You can't hold any more weapons" )
			return
		end

		UI.newAlert("Enjoy your new "..outfit.." system for "..price.." credits")
		PLAYER:AddWeapon(outfit)
		PLAYER:SetCredits( player_credits - price )
		HUD.newStatus(outfit..":", 130, UPPER_LEFT, string.format("playerAmmo(%q)",outfit))
	elseif ( Set(Epiar.engines())[outfit] ) then
		--print("Engine...")
		if PLAYER:GetEngine() ~= outfit then
			PLAYER:SetEngine(outfit)
			PLAYER:SetCredits( player_credits - price )
			UI.newAlert("Enjoy your new "..outfit.." system for "..price.." credits.")
		else
			UI.newAlert("You already have a "..outfit.." system.")
		end
	elseif ( Set(Epiar.outfits())[outfit] ) then
		--print("Outfit...")
		PLAYER:AddOutfit(outfit)
		PLAYER:SetCredits( player_credits - price )
		UI.newAlert("Enjoy your new "..outfit.." system for "..price.." credits")
	else
		print("Unknown Outfit: "..outfit)
		return
	end
	--print("Outfit Purchase complete")
end

function sellOutfit(outfit)
	if outfit==nil then
		local viewerLabel = UI.search("/Window/Tabs'Store'/'Outfitting'/Frame[1]/Label[0]/")
		if viewerLabel ~= nil then
			outfit = viewerLabel:GetText()
			--print('Selling ',outfit)
		else
			print('Could not interpret nil model for buyShip()')
			return
		end
	end

	--print("Removing Outfit...")

	if ( Set(Epiar.weapons())[outfit] ) then
		--print("Weapon...")
		local weaponsAndAmmo = PLAYER:GetWeaponsAmmo()
		if weaponsAndAmmo[outfit]~=nil then
			PLAYER:RemoveWeapon(outfit)
			HUD.closeStatus(outfit..":");
		else
			UI.newAlert("You don't have a "..outfit.."!")
			return
		end
		local weaponInfo = Epiar.getWeaponInfo(outfit)
		if weaponInfo["Ammo Consumption"] ~= 0 then
			-- Do something about selling a weapon that has ammo. I'm thinking
			-- it should remove 100 ammo with each sale, and RemoveAmmo()
			-- should set ammo to zero if it would have gone negative.
			--PLAYER:RemoveAmmo(outfit,100)
			-- Or, better yet, ammunition should be bought and sold separately.
		end
	elseif ( Set(Epiar.engines())[outfit] ) then
		--print("Engine...")
		UI.newAlert("You can't sell your engines!")
		return

	elseif ( Set(Epiar.outfits())[outfit] ) then
		--print("Outfit...")
		local playerOutfits = PLAYER:GetOutfits()

		local found = false

		for n,po in pairs(playerOutfits) do
			if po == outfit then
				if found == false then PLAYER:RemoveOutfit(outfit) end
				found = true
			end
		end

		if found == false then
			UI.newAlert("You don't have a "..outfit.."!")
			return
		end
		
	else
		print("Unknown Outfit: "..outfit)
		return
	end

	local price = Epiar.getMSRP(outfit)
	local adjustedPrice = math.floor( price * 0.65) -- only get back 65% of MSRP

	local player_credits = PLAYER:GetCredits()

	print("Crediting your account...")

	PLAYER:SetCredits( player_credits + adjustedPrice )
	UI.newAlert("You sold your "..outfit.." system for "..adjustedPrice.." credits")
	print("Outfit Selling complete")
end

--- Trade a Commodity
function tradeCommodity(transaction, commodity, count, price)
	local player_credits = PLAYER:GetCredits()
	local cargo,stored,storable = PLAYER:GetCargo()
	--print("Trading...")
	if transaction=="buy" then
		--print("Tonnage available:",storable-stored)
		--print("Tonnage requested:",count)
		--print("Tonnage affordable:",player_credits/price)
		trueCount = math.min(storable-stored,count,math.floor(player_credits/price)) -- Can't buy more than this
		stored = PLAYER:StoreCommodities( commodity,trueCount )
		if(stored ~= trueCount) then
			UI.newAlert("ARG! That wasn't supposed to happen!")
		end
		PLAYER:SetCredits( player_credits - trueCount*price )
		HUD.newAlert(string.format("You bought %d tons of %s for %d credits",trueCount,commodity,price*trueCount))
	elseif transaction=="sell" then
		-- print("Tonnage stored:",cargo[commodity] or 0)
		-- print("Tonnage requested:",count)
		trueCount = math.min(count,cargo[commodity] or 0) -- Can't sell more than this
		--print("Discarding "..trueCount.." Tonnes")
		discarded = PLAYER:DiscardCommodities( commodity,trueCount )
		--print("Discarded "..discarded.." Tonnes")
		if(discarded ~= trueCount) then
			UI.newAlert("ARG! That wasn't supposed to happen!")
		end
		PLAYER:SetCredits( player_credits + trueCount*price )
		HUD.newAlert(string.format("You sold %d tons of %s for %d credits",trueCount,commodity,price*trueCount))
	else
		error( string.format( "Sorry, trading Commodities doesn't understand transaction %q", transaction ) )
	end
	
	local commodityBox = UI.search(string.format("/Window/'Store'/'Trade'/Textbox%q/", commodity))
	if commodityBox ~= nil then
		local cargo,stored,storable = PLAYER:GetCargo()
		commodityBox:setText( cargo[commodity] or 0)
	end

	--print "Done Trading..."
	return 1
end

function storeView(containerPath, itemType, itemName )
	local getters = {
		["ship"]=Epiar.getModelInfo,
		["weapon"]=Epiar.getWeaponInfo,
		["engine"]=Epiar.getEngineInfo,
		["outfit"]=Epiar.getOutfitInfo,
	}
	local iteminfo = getters[itemType](itemName)

	-- Reset the Item Viewer
	local viewer = UI.search( containerPath .. "Frame[1]/")
	if viewer ~= nil then viewer:close() end
	viewer = UI.newFrame( width*.25+20, 10, width*.75-50, height-150)
	UI.search( containerPath ):add( viewer )

	viewer:add( UI.newLabel(195, 15, iteminfo["Name"], 1) )
	viewer:add( UI.newLabel(195, 35, iteminfo["MSRP"] .. " Credits", 1) )

	local yoff = 50
	local pic = UI.newPicture(10, yoff, 135, 135, iteminfo["Picture"] or iteminfo["Image"])
	pic:setPictureCenter(100, yoff + 60)
	viewer:add( pic )

	for statname,value in pairs(iteminfo) do
		--print(statname, value )
		-- Skip these kinds
		if statname == "Name"
		or statname == "Description"
		or statname == "MSRP"
		or statname == "Picture"
		or statname == "Image"
		or statname == "Sound"
		or statname == "Animation"
		or statname == "Engine"
		or type(value) == "table"
		or value == 0
		then
			-- Do Nothing
		else
			if type(value)=="number" and math.floor(value) ~= value then
				value = string.format("%.2f", value)
			end
			viewer:add( UI.newLabel(220, yoff, statname) )
			viewer:add( UI.newLabel(330, yoff, value) )
			yoff = yoff + 21
		end
	end
	yoff = 250
	local desc = UI.newParagraph( 10, yoff, width * 0.75 - 70, 300, iteminfo.Description )
	viewer:add( desc )
	yoff = yoff + desc:GetH() + 10
	if iteminfo.Engine ~= nil then
		viewer:add( UI.newLabel(10, yoff, "Engine") )
		viewer:add( UI.newLabel(130, yoff, iteminfo.Engine) )
		yoff = yoff + 21
	end
	-- The Weaponslots code needs a rewrite
	if iteminfo.weaponSlots ~= nil then
		for weaponnum = 0 , (-1 + iteminfo.weaponSlots.length) do
			viewer:add( UI.newLabel(10, yoff, iteminfo.weaponSlots[""..weaponnum].name ) )
			viewer:add( UI.newLabel(130, yoff, iteminfo.weaponSlots[""..weaponnum].content or "(Empty)") )
			yoff = yoff + 21
		end
	end
end

