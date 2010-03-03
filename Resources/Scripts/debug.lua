-- Use this script for a solar system

infoWindows = {}
function infoTable(info,win)
	y1,y2=55,40
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

function infoTableCollect(info,uiElements)
	for title, value in pairs(info) do
		if uiElements[title]~=nil then
			info[title] = uiElements[title]:GetText()
		end
	end
end

function showInfo()
	currentTarget = HUD.getTarget()
	sprite = Epiar.getSprite(currentTarget)
	spritetype = sprite:GetType()
	if spritetype == 1 then -- planet
		showPlanetInfo()
	elseif (spritetype == 4) or (spritetype == 8) then -- Ship or Player
		showModelInfo(sprite)
	else
		io.write(string.format("Cannot show info for sprite of type [%d]\n",spritetype))
	end
end

function saveInfo(saveFunc,name)
	if infoWindows[name] == nil then return end
	local info = infoWindows[name].info
	local texts = infoWindows[name].texts
	local win = infoWindows[name].win
	infoTableCollect(info, texts)
	saveFunc(info);
	win:close();
	win=nil
	infoWindows[name]=nil
	print("Saved "..name)
end

function showPlanetInfo()
	currentTarget = HUD.getTarget()
	if SHIPS[currentTarget] ~= nil then return end
	planet = Epiar.getSprite(currentTarget)
	planetName = planet:Name()
	if infoWindows[planetName]~= nil then return end
	
	local planetInfo = Epiar.getPlanetInfo( currentTarget )
	local planetInfoWin = UI.newWindow( 50,100,200,400, "Planet Info: "..planetName)
	local infoTexts = infoTable(planetInfo,planetInfoWin)
	planetInfoWin:add(UI.newButton( 80,350,100,30,"Save", "saveInfo(Epiar.setPlanetInfo,'"..planetName.."')" ))
	infoWindows[planetName] = {win=planetInfoWin, info=planetInfo, texts=infoTexts}
end

function showModelInfo(ship)
	modelName = ship:GetModelName()
	if infoWindows[modelName] ~= nil then return end
	
	modelInfo = Epiar.getModelInfo( modelName )
	modelInfoWin = UI.newWindow( 50,100,200,400, "Model Info: "..modelName)
	infoTexts = infoTable(modelInfo,modelInfoWin)
	infoWindows[modelName] = {win=modelInfoWin, info=modelInfo, texts=infoTexts}

	weaponsAndAmmo = ship:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		modelInfoWin:add(UI.newLabel( 10, y1, weapon))
		modelInfoWin:add(UI.newTextbox( 90, y2, 60, 1, ammo))
		modelInfoWin:add(UI.newButton( 150, y2, 40, 20, "-->", "showWeaponInfo('"..weapon.."')"))
		y1,y2=y1+yoff,y2+yoff
	end
	
	modelInfoWin:add(UI.newButton( 80,350,100,30,"Save", "saveInfo(Epiar.setModelInfo,'"..modelName.."')" ))
end

function showWeaponInfo(weaponName)
	if infoWindows[weaponName] ~= nil then return end

	weaponInfo = Epiar.getWeaponInfo( weaponName )
	weaponInfoWin = UI.newWindow( 50,100,200,400, "Weapon Info: "..weaponName)
	local infoTexts = infoTable(weaponInfo,weaponInfoWin)
	weaponInfoWin:add(UI.newButton( 80,350,100,30,"Close", "saveInfo(Epiar.setWeaponInfo,'"..weaponName.."')" ))
	infoWindows[weaponName] = {win=weaponInfoWin, info=weaponInfo, texts=infoTexts}
end

function showEngineInfo(engineName)
	if infoWindows[engineName] then return end
	engineInfo = Epiar.getEngineInfo( engineName )
	engineInfoWin = UI.newWindow( 50,100,200,400, "Engine Info: "..engineName)
	local infoTexts = infoTable(engineInfo,engineInfoWin)
	engineInfoWin:add(UI.newButton( 80,350,100,30,"Close", "saveInfo(Epiar.setEngineInfo,'"..enginName.."')" ))
	infoWindows[weaponName] = {win=engineInfoWin, info=engineInfo, texts=infoTexts}
end

