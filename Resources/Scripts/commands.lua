-- SDL Key Translations
Key = {
	backspace=8, tab=9, clear=12, pause=19, escape=27, space=32,
	up=273, down=274, right=275, left=276, insert=277, home=278, pageup=280, pagedown=281,
	-- Should we support the function keys?
	f1=282, f2=283, f3=284, f4=285, f5=286, f6=287, f7=288, f8=289, f9=290, f10=291, f11=292, f12=293, f13=294, f14=295, f15=296,
	numlock=300, capslock=301, scrolllock=302, rshift=303, lshift=304, rctrl=305, lctrl=306, ralt=307, lalt=308, rmeta=309, lmeta=310, lsuper=311, rsuper=312,
	-- Special Lua keywords
	["return"]=13, ["end"]=279
}

--- Translates key
function sdlkey(k)
	if Key[k] then
		return Key[k]
	else
		return k
	end
end

--- Commands that are currently registered
commands = {}

--- Commands that should always be registered
defaultCommands = {
	-- Each command should be a table
	-- { KEY, TITLE, SCRIPT, KEYMODE }
	{'?', "Game Options", "options()",KEYTYPED},
	}

--- Register multiple commands
function registerCommands(cmds)
    for i,cmd in pairs(cmds) do
		-- Remember this command for later
        table.insert(commands,cmd)
		-- Register this command to the C Engine
		keyval, name, code, keymode = cmd[1],cmd[2],cmd[3],cmd[4]
		Epiar.UnRegisterKey(sdlkey(keyval), keymode)
		Epiar.RegisterKey(sdlkey(keyval), keymode, code)
    end
end

registerCommands(defaultCommands)

--- Specify keys configuration
function chooseKeys()
	off_x,off_y = 30,30
	w,h = 300,(#commands*20 + 80)
	keyinput = {}
	labels = {}
	--
	if UI.search("/'Possible Keys'/") ~=nil then return end
	keywin:add(UI.newButton(w/2-120/2, h-38, 120, 30, "Save Changes", "processKeyWin(commands)"))
	keywin:add(UI.newButton(10, h-38, 30, 30, "?", "keyhelp()"))
end

--- Process the key configuration

--- Help window
function keyhelp()
	Epiar.pause()
	if UI.search("/'Possible Keys'/") ~=nil then return end
	off_x,off_y = 20,20
	w,h = 130,50
	-- This seems to be the only way to count keys in a dictionary.
	for k,v in pairs(Key) do h=h+20 end
	-- TODO: Make this a scrollable window
	local keyhelpwin = UI.newWindow( 100, 100, w, 300, "Possible Keys")
	for k,v in pairs(Key) do
		off_y = off_y + 20
		keyhelpwin:add( UI.newLabel(off_x,off_y,"- "..k) )
	end
	keyhelpwin:addCloseButton()
end
