-- Use this script for a solar system
infoWindows = {}
componentWins = {}

--- View components
function componentDebugger()
	UI.add(
		UI.newButton(  0,0,100,30,"Alliance","componentViewer('Alliance',Epiar.alliances,'Epiar.getAllianceInfo')" ),
		UI.newButton(100,0,100,30,"Commodity","componentViewer('Commodity',Epiar.commodities,'Epiar.getCommodityInfo')" ),
		UI.newButton(200,0,100,30,"Engine","componentViewer('Engine',Epiar.engines,'Epiar.getEngineInfo')" ),
		UI.newButton(300,0,100,30,"Model","componentViewer('Model',Epiar.models,'Epiar.getModelInfo')" ),
		UI.newButton(400,0,100,30,"Planet","componentViewer('Planet',Epiar.planetNames,'Epiar.getPlanetInfo')" ),
		UI.newButton(500,0,100,30,"Gate","componentViewer('Gate',Epiar.gateNames,'Epiar.getGateInfo')" ),
		UI.newButton(600,0,100,30,"Technology","technologyViewer()"),
		UI.newButton(700,0,100,30,"Weapon","componentViewer('Weapon',Epiar.weapons,'Epiar.getWeaponInfo')" ),
		UI.newButton(800,0,100,30,"Outfit","componentViewer('Outfit',Epiar.outfits,'Epiar.getOutfitInfo')" ),
    	UI.newButton(WIDTH/2-50, HEIGHT-30, 100, 30, "Save Components", "Epiar.saveComponents()" )
	)
end

--- The EditorLayouts describe the ordering and type of the component attributes
--- This could be hardcoded into the c++ engine

AllianceEditorLayout = {
	{"Name", "String"},
	{"Aggressiveness", "Number"},
	{"AttackSize", "Integer"},
	{"Color", "String"},
	--{"Illegal", "List"},
	}

CommodityEditorLayout = {
	{"Name", "String"},
	{"MSRP", "Number"},
	}

EngineEditorLayout = {
	{"Name", "String"},
	--{"Animation", "Animation"}, -- Animation Picker
	{"Picture", "Picture"}, -- Picture Picker
	{"MSRP", "Integer"},
	{"Force", "Integer"},
	{"Fold Drive", "Integer"},
	}

ModelEditorLayout = {
	{"Name", "String"},
	{"Image", "Picture"}, -- Picture Picker
	{"Rotation", "Number"},
	{"MSRP", "Integer"},
	{"Thrust", "Integer"},
	{"Mass", "Number"},
	{"MaxHull", "Integer"},
	{"MaxShield", "Integer"},
	{"MaxSpeed", "Integer"},
	{"Cargo", "Integer"},
	--{"Engine", "Engine"}, -- Engine Picker (Dropdown of available Engines)
	{"weaponSlots", "Weapon slots"}, -- Slot configuration builder
	}

PlanetEditorLayout = {
	{"Name", "String"},
	{"X", "Integer"},
	{"Y", "Integer"},
	{"Image", "Picture"}, -- Picture Picker
	{"Alliance", "String"}, -- TODO: Should be Alliance Picker (Dropdown of available Alliances)
	{"Landable", "Integer"},
	{"Traffic", "Integer"},
	{"Militia", "Integer"},
	{"Influence", "Integer"},
	{"Technologies", "Technologies"},
	}

GateEditorLayout = {
	{"Name", "String"},
	{"X", "Integer"},
	{"Y", "Integer"},
	{"Exit", "String"},
	}

WeaponEditorLayout = {
	{"Name", "String"},
	{"Picture", "Picture"}, -- Picture Picker
	{"Image", "Picture"}, -- Picture Picker
	{"MSRP", "Integer"},
	{"Payload", "Integer"},
	{"Velocity", "Integer"},
	{"Lifetime", "Integer"},
	{"FireDelay", "Integer"},
	{"Type", "Integer"},
	{"Tracking", "Number"},
	{"Ammo Type", "Integer"},
	{"Ammo Consumption", "Integer"},
	{"Sound", "String"}, -- TODO: Should be Sound Picker
	}

OutfitEditorLayout = {
	{"Name", "String"},
	{"Picture", "Picture"}, -- Picture Picker
	{"MSRP", "Integer"},
	{"MaxSpeed", "Number"},
	{"Force", "Number"},
	{"Rotation", "Number"},
	{"MaxHull", "Integer"},
	{"MaxShield", "Integer"},
	{"Cargo", "Integer"},
	{"SurfaceArea", "Integer"},
	{"Mass", "Number"},
	}

EditorLayouts = {
	Alliance = AllianceEditorLayout,
	Commodity = CommodityEditorLayout,
	Engine = EngineEditorLayout,
	Model = ModelEditorLayout,
	Planet = PlanetEditorLayout,
	Gate = GateEditorLayout,
	Weapon = WeaponEditorLayout,
	Outfit = OutfitEditorLayout,
}

--- Creates a generic list of Component buttons
-- TODO: This window should have an "Add Component" button
function componentViewer(kind,listFunc,getStr)
	if UI.search( string.format("/Window%q/", kind) ) ~= nil then return end
	list = listFunc()
	componentWins[kind] = UI.newWindow(10,100,140,(#list)*30+90,kind)
	for i = 1,#list do
		s = list[i]
		componentWins[kind]:add( UI.newButton(10,i*30,120,30,s,string.format("showComponent(%q,%q,%s)",kind,s,getStr)))
	end
    componentWins[kind]:add( UI.newButton(10,#list*30+40,120,30,"NEW",string.format("showComponent(%q,%q,%s)",kind,'',getStr)))
	componentWins[kind]:add( UI.newButton(115,5,15,15,"X", string.format("componentWins[%q]:close();componentWins[%q]=nil",kind,kind)))
end

function showComponent(kind,name,getterFunc)
	if UI.search( string.format("/Window%q/", name) ) ~= nil then return end
	local height=700
	local width=250
	local theInfo = getterFunc( name )
	local theWin = UI.newWindow(150,100,width,height,name,
		UI.newButton( 15,5,15,15,"X", string.format("infoWindows[%q].win:close();infoWindows[%q]=nil",name,name)))

    if kind=="Planet" and name~="" then
        planet = Planet.Get(name)
        Epiar.focusCamera(planet:GetID())
    elseif kind=="Gate" and name~="" then
        Epiar.focusCamera(theInfo.X, theInfo.Y)
    end
	
	local theFields = {}
	local thePics = {}
	local theWeaponTables = {}
	yoff=40 -- Buffer for the titlebar?
	for i,layout in ipairs(EditorLayouts[kind]) do
		local title,fieldType = layout[1],layout[2]
		local field = nil
		local value = theInfo[title]
		if fieldType == "String" then
			theWin:add(UI.newLabel( 10, yoff, title..":"))
			field = UI.newTextbox( 90, yoff, 100, 1, value)
			theWin:add(field)
			yoff = yoff+20
		elseif fieldType == "Integer" then
			theWin:add(UI.newLabel( 10, yoff, title..":"))
			field = UI.newTextbox( 90, yoff, 100, 1, value)
			theWin:add(field)
			yoff = yoff+20
		elseif fieldType == "Number" then
			if math.floor(value) ~= value then
				value = string.format("%.2f",value)
			end
			theWin:add(UI.newLabel( 10, yoff, title..":"))
			field = UI.newTextbox( 90, yoff, 100, 1, value)
			theWin:add(field)
			yoff = yoff+20
		elseif fieldType == "Picture" then
			theWin:add(UI.newLabel( 10, yoff+10, title..":"))
			yoff = yoff+35
			local pic = UI.newPicture( 10, yoff, width-20, 100, value)
			theWin:add(pic)
			thePics[title] = pic
			yoff = yoff+100
			field = UI.newTextbox( 10, yoff,width-30,1, value)
			theWin:add(field)
			yoff = yoff+20
			theWin:add(UI.newButton( 10, yoff,width-30,20,"Select Image", string.format("ImagePicker(%q,%q)",name,title)))
			yoff = yoff+20+5
		elseif fieldType == "Technologies" then
			theWin:add(UI.newLabel( 10, yoff+10, title..":"))
			yoff = yoff+35
			technologies = Epiar.technologies()
			local knownTechs = {}
			for i = 1,#technologies do
				local tech = technologies[i]
				knownTechs[tech] = 0
			end
			for i = 1,#value do
				knownTechs[value[i]] = 1
			end
			field = {}
			for i = 1,#technologies do
				local tech = technologies[i]
				field[tech] = UI.newCheckbox( 10,yoff,knownTechs[tech],tech)
				theWin:add(field[tech])
				yoff = yoff+20
			end
		elseif fieldType == "Weapon slots" then

			theWin:add(UI.newLabel( 10, yoff+10, title..":"))
			yoff = yoff+35

			theWin:add(UI.newButton( 10, yoff,width-30,20,"Edit weapon slots...", string.format("EditWeaponSlots(%q, %q)",name,title)))
			yoff = yoff+20+5

			theInfo[title]["desiredLength"] = 16
			theInfo[title]["filler"] = {enabled="no", name="", mode="auto", x=0, y=0, angle=0, motionAngle=0, content="", firingGroup=0}
			theWeaponTables[title] = theInfo[title]
		else
			print("Hmmm, it looks like '",fieldType,"' hasn't been implemented yet.")
		end
		theFields[title] = field
	end
	
	theWin:add( UI.newButton( 80,yoff+20,100,30,"Save", string.format("saveInfo(%q)",name )) )
	infoWindows[name] = {kind=kind,win=theWin, info=theInfo, texts=theFields,pics=thePics, weapontables=theWeaponTables}
end

-- see the further developed infoTable function later in this file

--- Lays out a series of labels and textboxes
-- Returns the checkboxes for later
-- TODO This makes WAY too many assumptions about the size and shape of the window
-- TODO The C++ engine should be able to auto-arrange these for us.
--function infoTable(info,win)
--	local y1,y2=155,140
--	local yoff=20
--	uiElements = {}
--	for title, value in pairs(info) do
--		-- Truncate decimal numbers to only 2 digits
--		if type(value)=="number" and math.floor(value) ~= value then
--			value = string.format("%.2f",value)
--		end
--		win:add(UI.newLabel( 10, y1, title))
--		uiElements[title] = UI.newTextbox( 90, y2, 100, 1, value)
--		win:add(uiElements[title])
--		y1,y2=y1+yoff,y2+yoff
--	end
--	return uiElements
--end

--- Show Info for the current Target
function showInfo()
	currentTarget = HUD.getTarget()
    sprite = Epiar.getSprite(currentTarget)
	spritetype = sprite:GetType()
	if spritetype == 1 then -- planet
        showComponent("Planet",sprite:GetName(),Epiar.getPlanetInfo)
	elseif (spritetype == 4) or (spritetype == 8) then -- Ship or Player
		showShipInfo(sprite)
	else
		io.write(string.format("Cannot show info for sprite of type [%d]\n",spritetype))
	end
end

--- Saves information
function saveInfo(name)
	if UI.search( string.format("/Window%q/", name) ) == nil then return end
	local info = infoWindows[name].info
	local texts = infoWindows[name].texts
	local weapontables = infoWindows[name].weapontables
	local win = infoWindows[name].win
	local kind = infoWindows[name].kind
	for i,layout in ipairs(EditorLayouts[kind]) do
		local title,fieldType = layout[1],layout[2]
		local field = texts[title]
		local original = info[title]
		if texts[title]~=nil then
			if fieldType == "String"
			or fieldType == "Integer"
			or fieldType == "Number"
			or fieldType == "Picture" then
				info[title] = texts[title]:GetText()
			elseif fieldType == "Technologies" then
				local techs = {}
				for tech, box in pairs(texts[title]) do
					if box:IsChecked() then
						table.insert(techs, tech )
					end
				end
				info[title] = techs
			else
				print("Hmmm, it looks like '"..fieldType.."' hasn't been implemented yet.")
			end
		elseif fieldType == "Weapon slots" then
			info[title] = weapontables[title]
		end
	end
	Epiar.setInfo(kind,info)
	win:close()
	win=nil
	infoWindows[name]=nil
	print("Saved "..name)
end

--- View technology
function technologyViewer()
	if UI.search( "/Window'Technologies'/" ) ~= nil then return end
	technologies = Epiar.technologies()
	technologiesWindow = UI.newWindow(10,100,140,(#technologies)*30+90,"Technologies")
	for i = 1,#technologies do
		name = technologies[i]
		technologiesWindow:add( UI.newButton(10,i*30,120,30,name,string.format("showTechInfo(%q)",name)))
	end
    technologiesWindow:add( UI.newButton(10,#technologies*30+40,120,30,"NEW","showTechInfo('')"))
	technologiesWindow:add( UI.newButton(115,5,15,15,"X","technologiesWindow:close();technologiesWindow=nil"))
end

--- Save technology
function saveTech(name)
	if UI.search( string.format("/Window%q/", name) ) == nil then return end
	local win = infoWindows[name].win
	local boxes = infoWindows[name].boxes
	local nameField = infoWindows[name].name
	local models,weapons,engines,outfits={},{},{},{}
	-- Gather the chosen techs into the correct lists
	for techGroup,boxset in pairs(boxes) do
		for tech,box in pairs(boxset) do
			if box:IsChecked() then
				if     techGroup=="Models"  then table.insert(models,tech)
				elseif techGroup=="Weapons" then table.insert(weapons,tech)
				elseif techGroup=="Engines" then table.insert(engines,tech)
				elseif techGroup=="Outfits" then table.insert(outfits,tech)
				end
			end
		end
	end
	-- Save these lists
	Epiar.setInfo('Technology',nameField:GetText(),models,weapons,engines,outfits)
	win:close()
	win=nil
	infoWindows[name]=nil
end

--- Show technology information
function showTechInfo(name)
	if UI.search( string.format("/Window%q/", name) ) ~= nil then return end
	local allmodels = Epiar.models()
	local allweapons = Epiar.weapons()
	local allengines = Epiar.engines()
	local alloutfits = Epiar.outfits()
	local techs = Epiar.getTechnologyInfo(name)
	local models,weapons,engines,outfits = techs[1],techs[2],techs[3],techs[4]
	local height = 50 + math.max(#allweapons,#allmodels,#allengines,#alloutfits)*20
	local width = 400
	local theWin = UI.newWindow(150,100,width,height,name)
	theWin:add(UI.newLabel( 15, 45, "Name:"))
	local nameField= UI.newTextbox( 90, 30, 200, 1, name)
	theWin:add(nameField)
	local optionTabs = UI.newTabCont( 10, 65, width-30, height-120,"Options Tabs")
	theWin:add(optionTabs)
	local knownTechs = {}
	checkedTechs = {}
	for i,t in ipairs({allmodels,allweapons,allengines,alloutfits}) do
		for j,s in ipairs(t) do knownTechs[s]=0 end
	end
	for i,t in ipairs({models,weapons,engines,outfits}) do
		for j,s in ipairs(t) do knownTechs[s]=1 end
	end
	function showTable(techGroup,techList)
		local thisTab = UI.newTab(techGroup)
		optionTabs:add(thisTab)
		checkedTechs[techGroup]={}
		for i,s in ipairs(techList) do
			checkedTechs[techGroup][s] = UI.newCheckbox(30, i*20,knownTechs[s],s)
			thisTab:add(checkedTechs[techGroup][s])
			--print(string.format("%s %d: %s %s",techGroup,i,s,(checkedTechs[techGroup][s]:IsChecked() and "YES" or "NO")))
			--TODO: Add tiny button to view/edit this technology
		end
	end
	showTable("Models",allmodels)
	showTable("Weapons",allweapons)
	showTable("Engines",allengines)
	showTable("Outfits",alloutfits)
	infoWindows[name] = {kind='Technology',win=theWin,name=nameField,boxes=checkedTechs}
	theWin:add( UI.newButton(width-25,5,15,15,"X",string.format("infoWindows[%q].win:close();infoWindows[%q]=nil",name,name)))
	theWin:add(UI.newButton(width-120,height-40,100,30,"Save", string.format("saveTech(%q)",name) ))
end

--- Show ship information
function showShipInfo(ship)
	shipID = ship:GetID()
	modelName = ship:GetModelName()
	Epiar.focusCamera(shipID)
	shipModel = ship:GetModelName()
	shipname = string.format("%s #%d",shipModel, shipID)
	if UI.search( string.format("/Window%q/", shipname) ) ~= nil then return end

	shipInfoWin = UI.newWindow(150,100,200,400,shipname)
	shipInfoWin:add( UI.newPicture( 20,25,160,100,shipModel))
	y1,y2=155,140
	yoff=20
	-- Model
	shipInfoWin:add(UI.newLabel( 10, y1, "Model:"))
	shipInfoWin:add(UI.newButton( 90, y2, 100, 20, shipModel, "showComponent('Model','"..shipModel.."',Epiar.getModelInfo)"))
	y1,y2=y1+yoff,y2+yoff
	-- Weapons
	weaponsAndAmmo = ship:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		shipInfoWin:add(UI.newLabel( 10, y1, weapon))
		shipInfoWin:add(UI.newTextbox( 90, y2, 60, 1, ammo))
		shipInfoWin:add(UI.newButton( 150, y2, 40, 20, "-->", "showComponent('Weapon','"..weapon.."',Epiar.getWeaponInfo)"))
		y1,y2=y1+yoff,y2+yoff
	end
	-- View the AI State Machine
	local machine, state = ship:GetState()
	shipInfoWin:add(UI.newLabel( 10, y1, "Machine:"))
	shipInfoWin:add(UI.newTextbox( 90, y2, 100, 1, machine))
	-- TODO The State doesn't get updated in real time.  Fix this somehow...
	y1,y2=y1+yoff,y2+yoff
	shipInfoWin:add(UI.newLabel( 10, y1, "State:"))
	shipInfoWin:add(UI.newTextbox( 90, y2, 100, 1, state))
	-- TODO Outfit?
	infoWindows[shipID] = {win=shipInfoWin, info={},texts={}}
	shipInfoWin:add(UI.newButton( 80,350,100,30,"Save", string.format("infoWindows[%d].win:close();infoWindows[%d]=nil",shipID,shipID) ))
end

function ImagePicker(name,title)
	if UI.search( "/Window'Image Picker'/" ) ~= nil then return end
	local imagePickerWin = UI.newWindow(700,150,250,700, "Image Picker")
	--TODO: Preserve the textbox assosciated with this window.
	--      When imagePick is called, set the textbox value to the image path

	function imagePick(name,title,path)
		if UI.search( "/Window'Image Picker'/" ) ==nil then return end
		infoWindows[name]["texts"][title]:setText( path )
		infoWindows[name]["pics"][title]:setPicture( path )
		UI.search( "/Window'Image Picker'/" ):close()
		print( "Picture Path:", path )
	end

	for i,picPath in pairs( Epiar.listImages()) do
		imagePickerWin:add(
			UI.newPicture(25,25+300*(i-1),200,200,"Resources/Graphics/"..picPath),
			UI.newButton( 25,225+300*(i-1),200,30, picPath,string.format("imagePick(%q,%q,%q)",name,title,"Resources/Graphics/"..picPath )))
	end
end

-- Modified version of the infoTable function
function infoTable(info, win, variables, fieldDesc, desiredSize)
	local y2=50
	local yoff=20

	local fieldWidth  = function(f) return fieldDesc[f][1] end
	local fieldType   = function(f) return fieldDesc[f][2] end
	local fieldOptions = function(f) return fieldDesc[f][3] end

	uiElements = {}

	function numformat (value)
		if type(value)=="number" and math.floor(value) ~= value then
			local formatted = string.format("%.2f",value)
			return formatted
		end
		return value
	end

	local xoff = 0

	local length = info["length"]
	local desiredLength = info["desiredLength"]

	for colNum =0,(info["fields"]-1) do
		local colKey = (string.format("%d", colNum))
		local title = variables[colKey]
		local w = fieldWidth(title)
		win:add(UI.newLabel( 10 + xoff, y2 - 20, title))
		xoff = xoff + w
	end

	-- add these rows from the bottom up so the active dropdown will not be obscured by other dropdown widgets
	y2 = yoff * desiredLength + 30
	for invRowNum =0,(desiredLength-1) do
		local rowNum = desiredLength - 1 - invRowNum

		local rowElements = {}

		rowKey = (string.format("%d", rowNum))

		local thisrow
		if rowNum <= (length-1) and info[rowKey]["enabled"] == "yes" then
			thisrow = info[rowKey]
		else
			if info[rowKey] ~= nil and info[rowKey]["enabled"] == "yes" then -- user enabled another row
				info["length"] = info["length"] + 1
				thisrow = info[rowKey]
			else
				-- after we have run out of actual data, fill up the remaining lines with a template
				-- which may or may not become part of the actual data depending on how the user edits it
				thisrow = info["filler"] 
			end
		end

		xoff = 0

		for colNum =0,(info["fields"]-1) do
			local colKey = (string.format("%d", colNum))
			local title = variables[colKey]
			local w = fieldWidth(title)
			local value = thisrow[title]
			if fieldType(title) == 'textbox' then
				rowElements[title] = UI.newTextbox( 10 + xoff, y2, w, 1, value)
			elseif fieldType(title) == 'dropdown' then
				rowElements[title] = UI.newDropdown( 10 + xoff, y2, w, yoff)
				for n,opt in pairs( fieldOptions(title) ) do
					rowElements[title]:addOption(opt)
				end
				--rowElements[title]:setValue(value)   -- need something like this for the Dropdown widget
			else
				-- no other types known at the moment
			end
			win:add(rowElements[title])
			xoff = xoff + w
		end

		--y2 = y2 + yoff
		y2 = y2 - yoff
		uiElements[rowKey] = rowElements
	end
	
	return uiElements
end

function EditWeaponSlots(name, title)
	SlotEditor = {}
	if UI.search( "/Window'Edit Weapon Slots'/" ) ~= nil then return end
	editWeaponSlotsWin = UI.newWindow(200,200,800,500, "Edit Weapon Slots")
	editWeaponSlotsWin:add( UI.newButton(5,5,15,15,"X","editWeaponSlotsWin:close();editWeaponSlotsWin=nil"))

	-- Grab the table
	local tab = infoWindows[name].weapontables[title]

	-- Tell the table interface function to make extra rows until there are 16
	tab["desiredLength"] = 16
	tab["filler"] = {enabled="no", name="", mode="auto", x=0, y=0, angle=0, motionAngle=0, content="", firingGroup=0}

	-- This ugly indexing trick is a workaround for Lua's apparent lack of sensible element ordering.
	-- Clean it up if a better built-in solution can be found.

	variables = { ["0"]="enabled", ["1"]="name", ["2"]="mode", ["3"]="x", ["4"]="y",
		      ["5"]="angle", ["6"]="motionAngle", ["7"]="content", ["8"]="firingGroup" }

	local contentOptions = Epiar.weapons()
	table.insert(contentOptions, 1, "(empty)")
	-- the description format for each field is: width, type, table of possible values (if applicable)
	local fieldDesc = {
		["enabled"]	= { 90, 'dropdown', {'yes','no'} },
		["name"]	= { 170,'textbox', nil },
		["mode"]	= { 70, 'dropdown', {'auto', 'manual'} },
		["x"]		= { 40, 'textbox', nil },
		["y"]		= { 40, 'textbox', nil },
		["angle"]	= { 50, 'textbox', nil },
		["motionAngle"]	= { 75, 'textbox', nil },
		["content"]	= { 100,'dropdown', contentOptions }, -- should change this to be a dropdown of all weapons + empty
		["firingGroup"]	= { 75, 'dropdown', {0, 1} }
	}

	fieldTable = infoTable( tab, editWeaponSlotsWin, variables, fieldDesc )

	editWeaponSlotsWin:add(UI.newButton( 150,400,100,30, "Finish", (string.format("finishEditingWeaponSlots(\"%s\", \"%s\", %d, %d)", name, title, tab["desiredLength"], tab["fields"]) ) ) )

	local imageName = infoWindows[name].texts["Image"]:GetText()

	SlotEditor["calcPosX"] = 350
	SlotEditor["calcPosY"] = 400
	editWeaponSlotsWin:add( UI.newLabel(SlotEditor["calcPosX"] - 60, SlotEditor["calcPosY"] - 20, "Click on the image to extract X,Y offsets for that position.") )
	local image = UI.newPicture(SlotEditor["calcPosX"], SlotEditor["calcPosY"], imageName)
	local imageX, imageY, imageW, imageH = image:GetEdges()
	SlotEditor["imageHH"] = imageH / 2
	SlotEditor["imageHW"] = imageW / 2
	SlotEditor["calcXLabel"] = UI.newLabel(SlotEditor["calcPosX"] - 40, SlotEditor["calcPosY"], "X: ")
	SlotEditor["calcYLabel"] = UI.newLabel(SlotEditor["calcPosX"] - 40, SlotEditor["calcPosY"]+15, "Y: ")
	image:setLuaClickCallback('calculateSlotOffset')
	editWeaponSlotsWin:add(image, SlotEditor["calcXLabel"], SlotEditor["calcYLabel"])
end

function calculateSlotOffset(x, y)
	local newY = x - SlotEditor["calcPosX"] - SlotEditor["imageHW"] -- flipping x and y here is intentional
	local newX = y - SlotEditor["calcPosY"] - SlotEditor["imageHH"] -- (the image is sideways)
	SlotEditor["calcXLabel"].setText(SlotEditor["calcXLabel"], (string.format("X: %d", newX)))
	SlotEditor["calcYLabel"].setText(SlotEditor["calcYLabel"], (string.format("Y: %d", newY)))
end

function finishEditingWeaponSlots(name, title, desiredLength, fields)
	for rowNum =0,(desiredLength-1) do
		local r = {}
		local rowKey = string.format("%d", rowNum)
		for colNum =0,(fields-1) do
			local fieldName = variables[(string.format("%d", colNum))]
			local value = fieldTable[(string.format("%d",rowNum))][fieldName]:GetText()
			if value == '(empty)' then value = '' end
			r[fieldName] = value
			if infoWindows[name].weapontables[title][rowKey] == nil then
				infoWindows[name].weapontables[title][rowKey] = {}
			end
			infoWindows[name].weapontables[title][rowKey][fieldName] = value
		end
	end
	
	editWeaponSlotsWin:close()
	editWeaponSlotsWin = nil
	variables = nil
	fieldTable = nil
	uiElements = nil
	SlotEditor = nil
end
	

function goto(x,y)
	Epiar.focusCamera(x,y)
end

function gotoButton()
	goto( UI.search("/'Go to Location'/Textbox[0]/"):GetText(),
          UI.search("/'Go to Location'/Textbox[1]/"):GetText() )
	UI.search("/'Go to Location'/"):close()
end

function gotoCommand()
	if UI.search( "/'Go to Location'/" ) ~= nil then gotoButton(); return end
	local cx,cy = Epiar.getCamera()
	local width = 160
	local height = 100
	UI.newWindow( WIDTH/2-100, HEIGHT/2-100, width, height, "Go to Location",
		UI.newLabel(10,30,"X"), UI.newTextbox(20,30,50,1,cx),
		UI.newLabel(90,30,"Y"), UI.newTextbox(100,30,50,1,cy),
		UI.newButton(width/2-40,55,80,30,"Go","gotoButton()"))
end

DX,DY = 20,20

debugCommands = {
	-- Each command should be a table
	-- { KEY, TITLE, SCRIPT }
	{'left', "Pan Left", "Epiar.moveCamera(-DX,0)",KEYPRESSED},
	{'down', "Pan Down", "Epiar.moveCamera(0,-DY)",KEYPRESSED},
	{'up', "Pan Up", "Epiar.moveCamera(0,DY)",KEYPRESSED},
	{'right', "Pan Right", "Epiar.moveCamera(DX,0)",KEYPRESSED},
	{'i', "Get Info", "showInfo()",KEYTYPED},
	{'space', "Go To", "gotoCommand()",KEYTYPED},
}
registerCommands(debugCommands)

-- Zoom keys
function debugZoomKeys()
    for k =1,9 do
        local keyval = string.byte(k)
        local vis = 1000*math.pow(2,k-1)
        local delta = 10*math.pow(2,k-1)
        local cmd = string.format("DX,DY=%f,%f;HUD.setVisibity(%d)",delta,delta,vis)
        Epiar.RegisterKey(keyval, KEYTYPED, cmd)
    end
end
debugZoomKeys()

