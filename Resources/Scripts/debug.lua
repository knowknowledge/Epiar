-- Use this script for a solar system
infoWindows = {}
componentWins = {}

function componentDebugger()
	if componentWindow ~= nil then return end
	componentWindow = UI.newWindow( 200,10,620,70, "Game Component Debugging",
		UI.newButton( 10,30,100,30,"Alliances","componentViewer('Alliances',Epiar.alliances,'Epiar.getAllianceInfo','Epiar.setAllianceInfo')" ),
		UI.newButton(110,30,100,30,"Engines","componentViewer('Engines',Epiar.engines,'Epiar.getEngineInfo','Epiar.setEngineInfo')" ),
		UI.newButton(510,30,100,30,"Weapons","componentViewer('Weapons',Epiar.weapons,'Epiar.getWeaponInfo','Epiar.setWeaponInfo')" ),
		UI.newButton(210,30,100,30,"Models","componentViewer('Models',Epiar.models,'Epiar.getModelInfo','Epiar.setModelInfo')" ),
		-- TODO: The above requires more getter/setter passing than should be required
		--       Can we merge the c++ engine getter/setters into one function and have them detect the Component Type?
		--       Notice how the buttons sometimes pass strings and sometimes pass function objects. Yuck. This will cause problems later.
		UI.newButton(310,30,100,30,"Planets","planetViewer()" ),
		UI.newButton(410,30,100,30,"Technologies","technologyViewer()")
	)
end
componentDebugger()

-- Creates a generic list of Component buttons
-- TODO: This window should have an "Add Component" button
function componentViewer(name,listFunc,getStr,setStr)
	if componentWins[name] ~= nil then return end
	list = listFunc()
	componentWins[name] = UI.newWindow(10,100,140,(#list)*30+50,name)
	for i = 1,#list do
		s = list[i]
		componentWins[name]:add( UI.newButton(10,i*30,120,30,s,string.format("showComponent('%s',%s,'%s')",s,getStr,setStr)))
	end
	componentWins[name]:add( UI.newButton(115,5,15,15,"X", string.format("componentWins['%s']:close();componentWins['%s']=nil",name,name)))
end

-- Creates an attribute setter for a particular Component
function showComponent(name,getterFunc,setterStr)
	if infoWindows[name] ~= nil then return end
	local theInfo = getterFunc( name )
	local theWin = UI.newWindow(150,100,200,400,name,
		UI.newPicture( 70,25,100,100,name),
		UI.newButton( 80,350,100,30,"Save", string.format("saveInfo(%s,'%s')",setterStr,name )),
		UI.newButton( 175,5,15,15,"X", string.format("infoWindows['%s'].win:close();infoWindows['%s']=nil",name,name)))
	local theTexts = infoTable(theInfo,theWin)
	infoWindows[name] = {win=theWin, info=theInfo, texts=theTexts}
end

-- Lays out a series of labels and textboxes
-- Returns the checkboxes for later
-- TODO This makes WAY too many assumptions about the size and shape of the window
-- TODO The C++ engine should be able to auto-arrange these for us.
function infoTable(info,win)
	y1,y2=155,140
	yoff=20
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

-- Show Info for the current Target
function showInfo()
	currentTarget = HUD.getTarget()
	spritetype = sprite:GetType()
	if spritetype == 1 then -- planet
		showPlanetInfo(currentTarget)
	elseif (spritetype == 4) or (spritetype == 8) then -- Ship or Player
		showShipInfo(sprite)
	else
		io.write(string.format("Cannot show info for sprite of type [%d]\n",spritetype))
	end
end

function saveInfo(saveFunc,name)
	if infoWindows[name] == nil then return end
	local info = infoWindows[name].info
	local texts = infoWindows[name].texts
	local win = infoWindows[name].win
	for title, value in pairs(info) do
		if texts[title]~=nil then
			info[title] = texts[title]:GetText()
		end
	end
	saveFunc(info);
	win:close();
	win=nil
	infoWindows[name]=nil
	print("Saved "..name)
end

function planetViewer()
	if planetsWindow ~= nil then return end
	planets = Epiar.planets()
	planetsWindow = UI.newWindow(10,100,140,(#planets)*30+50,"Planets")
	for i = 1,#planets do
		p = planets[i]
		planetsWindow:add( UI.newButton(10,i*30,120,30,p:Name(),string.format("showPlanetInfo(%d)",p:GetID())))
	end
	planetsWindow:add( UI.newButton(115,5,15,15,"X","planetsWindow:close();planetsWindow=nil"))
end

function showPlanetInfo(planetID)
	planet = Epiar.getSprite(planetID)
	planetName = planet:Name()
	Epiar.focusCamera(planetID)
	if infoWindows[planetName]~= nil then return end
	local planetInfo = Epiar.getPlanetInfo( planetID )
	local planetInfoWin = UI.newWindow(150,100,200,400, "Planet Info: "..planetName)
	local infoTexts = infoTable(planetInfo,planetInfoWin)
	-- TODO: Add checkboxes for different technologies
	planetInfoWin:add(UI.newButton( 80,350,100,30,"Save", "saveInfo(Epiar.setPlanetInfo,'"..planetName.."')" ))
	planetInfoWin:add(UI.newPicture( 70,25,100,100,planetName))
	planetInfoWin:add( UI.newButton(175,5,15,15,"X",string.format("infoWindows['%s'].win:close();infoWindows['%s']=nil;",planetName,planetName)))
	infoWindows[planetName] = {win=planetInfoWin, info=planetInfo, texts=infoTexts}
end

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

function showTechInfo(name)
	if infoWindows[name]~= nil then return end
	local allmodels = Epiar.models()
	local allweapons = Epiar.weapons()
	local allengines = Epiar.engines()
	local models,weapons,engines = Epiar.getTechnologyInfo(name)
	local height = 50 + math.max(#allweapons,#allmodels,#allengines)*20
	local theWin = UI.newWindow(150,100,600,height,name)
	local knownTechs = {}
	local checkedTechs = {}
	for i,t in ipairs({allmodels,allweapons,allengines}) do
		for j,s in ipairs(t) do knownTechs[s]=0 end
	end
	for i,t in ipairs({models,weapons,engines}) do
		for j,s in ipairs(t) do knownTechs[s]=1 end
	end
	function showTable(techName,techList,x)
		theWin:add(UI.newLabel(x,35,techName))
		for i,s in ipairs(techList) do
			--print(string.format("%s %d: %s",techName,i,s))
			checkedTechs[s] = UI.newCheckbox( x,30+i*20,knownTechs[s],s)
			theWin:add(checkedTechs[s])
			--TODO: Add tiny button to view/edit this technology
		end
	end
	showTable("Models",allmodels,50)
	showTable("Weapons",allweapons,200)
	showTable("Engines",allengines,350)
	infoWindows[name] = {win=theWin,boxes=checkedTechs}
	theWin:add( UI.newButton(575,5,15,15,"X",string.format("infoWindows['%s'].win:close();infoWindows['%s']=nil",name,name)))
	theWin:add(UI.newButton( 480,height-40,100,30,"Can't Save", string.format("infoWindows['%s'].win:close();infoWindows['%s']=nil;HUD.newAlert('Cannot Save Technologies yet...')",name,name) ))
end

function showShipInfo(ship)
	shipID = ship:GetID()
	modelName = ship:GetModelName()
	Epiar.focusCamera(shipID)
	if infoWindows[shipID] ~= nil then return end
	weaponsAndAmmo = ship:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		shipInfoWin:add(UI.newLabel( 10, y1, weapon))
		shipInfoWin:add(UI.newTextbox( 90, y2, 60, 1, ammo))
		shipInfoWin:add(UI.newButton( 150, y2, 40, 20, "-->", "showWeaponInfo('"..weapon.."')"))
		y1,y2=y1+yoff,y2+yoff
	end
	infoWindows[shipID] = {win=shipInfoWin, info={},texts={}}
	shipInfoWin:add(UI.newButton( 80,350,100,30,"Save", string.format("infoWindows[%d]:close();infoWindows[%d]=nil",shipID,shipID) ))
end

DX,DY = 20,20

debugCommands = {
	-- Each command should be a table
	-- { KEY, TITLE, SCRIPT }
	{'left', "Pan Left", "Epiar.moveCamera(-DX,0)",KEYPRESSED},
	{'down', "Pan Down", "Epiar.moveCamera(0,-DY)",KEYPRESSED},
	{'up', "Pan Up", "Epiar.moveCamera(0,DY)",KEYPRESSED},
	{'right', "Pan Right", "Epiar.moveCamera(DX,0)",KEYPRESSED},
	{'I', "Get Info", "showInfo()",KEYTYPED},
}
registerCommands(debugCommands)

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
