-- Use this script for a solar system
infoWindows = {}
componentWins = {}

--- View components
function componentDebugger()
	if componentWindow ~= nil then return end
	componentWindow = UI.newWindow( 150,10,740,70, "Game Component Debugging",
		UI.newButton( 10,30,100,30,"Alliance","componentViewer('Alliance',Epiar.alliances,'Epiar.getAllianceInfo')" ),
		UI.newButton(110,30,100,30,"Engine","componentViewer('Engine',Epiar.engines,'Epiar.getEngineInfo')" ),
		UI.newButton(210,30,100,30,"Model","componentViewer('Model',Epiar.models,'Epiar.getModelInfo')" ),
		UI.newButton(310,30,100,30,"Planet","componentViewer('Planet',Epiar.planetNames,'Epiar.getPlanetInfo')" ),
		UI.newButton(410,30,100,30,"Technology","technologyViewer()"),
		UI.newButton(510,30,100,30,"Weapon","componentViewer('Weapon',Epiar.weapons,'Epiar.getWeaponInfo')" ),
		UI.newButton(630,30,100,30,"Save","Epiar.saveComponents()" )
	)
end
componentDebugger()

--- Creates a generic list of Component buttons
-- TODO: This window should have an "Add Component" button
function componentViewer(kind,listFunc,getStr)
	if componentWins[kind] ~= nil then return end
	list = listFunc()
	componentWins[kind] = UI.newWindow(10,100,140,(#list)*30+50,kind)
	for i = 1,#list do
		s = list[i]
		componentWins[kind]:add( UI.newButton(10,i*30,120,30,s,string.format("showComponent('%s','%s',%s)",kind,s,getStr)))
	end
	componentWins[kind]:add( UI.newButton(115,5,15,15,"X", string.format("componentWins['%s']:close();componentWins['%s']=nil",kind,kind)))
end

--- Creates an attribute setter for a particular Component
function showComponent(kind,name,getterFunc)
    if kind=="Planet" then
        planet = Planet.Get(name)
        Epiar.focusCamera(planet:GetID())
    end
	if infoWindows[name] ~= nil then return end
	local theInfo = getterFunc( name )
	local theWin = UI.newWindow(150,100,200,400,name,
		UI.newPicture( 20,25,160,100,name),
		UI.newButton( 80,350,100,30,"Save", string.format("saveInfo('%s')",name )),
		UI.newButton( 10,350,60,30,"Pic", string.format("ImagePicker()",name )),
		UI.newButton( 175,5,15,15,"X", string.format("infoWindows['%s'].win:close();infoWindows['%s']=nil",name,name)))
	local theTexts = infoTable(theInfo,theWin)
	infoWindows[name] = {kind=kind,win=theWin, info=theInfo, texts=theTexts}
end

--- Lays out a series of labels and textboxes
-- Returns the checkboxes for later
-- TODO This makes WAY too many assumptions about the size and shape of the window
-- TODO The C++ engine should be able to auto-arrange these for us.
function infoTable(info,win)
	local y1,y2=155,140
	local yoff=20
	uiElements = {}
	for title, value in pairs(info) do
		-- Truncate decimal numbers to only 2 digits
		if type(value)=="number" and math.floor(value) ~= value then
			value = string.format("%.2f",value)
		end
		win:add(UI.newLabel( 10, y1, title))
		uiElements[title] = UI.newTextbox( 90, y2, 100, 1, value)
		win:add(uiElements[title])
		y1,y2=y1+yoff,y2+yoff
	end
	return uiElements
end

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
	if infoWindows[name] == nil then return end
	local info = infoWindows[name].info
	local texts = infoWindows[name].texts
	local win = infoWindows[name].win
	local kind = infoWindows[name].kind
	for title, value in pairs(info) do
		if texts[title]~=nil then
			info[title] = texts[title]:GetText()
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
	if technologiesWindow ~= nil then return end
	technologies = Epiar.technologies()
	technologiesWindow = UI.newWindow(10,100,140,(#technologies)*30+50,"technologies")
	for i = 1,#technologies do
		name = technologies[i]
		technologiesWindow:add( UI.newButton(10,i*30,120,30,name,string.format("showTechInfo('%s')",name)))
	end
	technologiesWindow:add( UI.newButton(115,5,15,15,"X","technologiesWindow:close();technologiesWindow=nil"))
end

--- Save technology
function saveTech(name)
	if infoWindows[name] == nil then return end
	local win = infoWindows[name].win
	local boxes = infoWindows[name].boxes
	local models,weapons,engines={},{},{}
	-- Gather the chosen techs into the correct lists
	for techGroup,boxset in pairs(boxes) do
		for tech,box in pairs(boxset) do
			if box:IsChecked() then
				if     techGroup=="Models"  then table.insert(models,tech)
				elseif techGroup=="Weapons" then table.insert(weapons,tech)
				elseif techGroup=="Engines" then table.insert(engines,tech)
				end
			end
		end
	end
	-- Save these lists
	Epiar.setInfo('Technology',name,models,weapons,engines)
	win:close()
	win=nil
	infoWindows[name]=nil
end

--- Show technology information
function showTechInfo(name)
	if infoWindows[name]~= nil then return end
	local allmodels = Epiar.models()
	local allweapons = Epiar.weapons()
	local allengines = Epiar.engines()
	local models,weapons,engines = Epiar.getTechnologyInfo(name)
	local height = 50 + math.max(#allweapons,#allmodels,#allengines)*20
	local theWin = UI.newWindow(150,100,600,height,name)
	local knownTechs = {}
	checkedTechs = {}
	for i,t in ipairs({allmodels,allweapons,allengines}) do
		for j,s in ipairs(t) do knownTechs[s]=0 end
	end
	for i,t in ipairs({models,weapons,engines}) do
		for j,s in ipairs(t) do knownTechs[s]=1 end
	end
	function showTable(techGroup,techList,x)
		theWin:add(UI.newLabel(x,35,techGroup))
		checkedTechs[techGroup]={}
		for i,s in ipairs(techList) do
			checkedTechs[techGroup][s] = UI.newCheckbox( x,30+i*20,knownTechs[s],s)
			theWin:add(checkedTechs[techGroup][s])
			--print(string.format("%s %d: %s %s",techGroup,i,s,(checkedTechs[techGroup][s]:IsChecked() and "YES" or "NO")))
			--TODO: Add tiny button to view/edit this technology
		end
	end
	showTable("Models",allmodels,50)
	showTable("Weapons",allweapons,200)
	showTable("Engines",allengines,350)
	infoWindows[name] = {kind='Technology',win=theWin,boxes=checkedTechs}
	theWin:add( UI.newButton(575,5,15,15,"X",string.format("infoWindows['%s'].win:close();infoWindows['%s']=nil",name,name)))
	theWin:add(UI.newButton( 480,height-40,100,30,"Save", string.format("saveTech('%s')",name) ))
end

--- Show ship information
function showShipInfo(ship)
	shipID = ship:GetID()
	modelName = ship:GetModelName()
	Epiar.focusCamera(shipID)
	if infoWindows[shipID] ~= nil then return end
	shipModel = ship:GetModelName()
	shipname = string.format("%s #%d",shipModel, shipID)
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

function ImagePicker(textBox)
	if imagePickerWin ~=nil then return end
	imagePickerWin = UI.newWindow(700,150,250,500, "Image Picker")
	--TODO: Preserve the textbox assosciated with this window.
	--      When imagePick is called, set the textbox value to the image path

	function imagePick(path)
		if imagePickerWin ==nil then return end
		imagePickerWin:close()
		imagePickerWin = nil
		print( "Picture Path:", path )
	end

	pics = Epiar.listImages()
	for i,picPath in ipairs(pics) do
		imagePickerWin:add(
			UI.newPicture(25,25+300*(i-1),200,200,"Resources/Graphics/"..picPath),
			UI.newButton( 25,225+300*(i-1),200,30, picPath,string.format("imagePick('%s')","Resources/Graphics/"..picPath )))
	end
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
}
registerCommands(debugCommands)

-- Zoom keys
function debugZoomKeys()
    for k =1,9 do
        local keyval = string.byte(k)
        local vis = 1000*math.pow(2,k-1)
        local delta = 10*math.pow(2,k-1)
        local cmd = string.format("DX,DY=%f,%f;HUD.setVisibity(%d)",delta,delta,vis)
        Epiar.RegisterKey(keyval, KEYPRESSED, cmd)
    end
end
registerInit(debugZoomKeys)
