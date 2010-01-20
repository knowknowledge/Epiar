-- Use this script for a solar system


-- Keyboard States:
KEYUP, KEYDOWN, KEYPRESSED, KEYTYPED = 0,1,2,3
SDLK_BACKSPACE, SDLK_ESCAPE = 9, 27
SDLK_RSHIFT, SDLK_LSHIFT = 47, 48
SDLK_UP, SDLK_DOWN, SDLK_RIGHT, SDLK_LEFT = 17, 18, 19, 20

--registerInit(store)
function createWindows()
	Epiar.RegisterKey('s',KEYTYPED,"store()")
	Epiar.RegisterKey('p',KEYTYPED,"togglePause()")
	Epiar.RegisterKey('g',KEYTYPED,"ui_demo()")
	-- pause should 1) not be implemented in lua and 2) should respond to keytyped events, not keydown events, else
	-- a 'p' typed into the UI will also pause the game. this makes no sense. however, if a UI text input has no
	-- focus, the UI will pass the typed event down the chain and pause should reach it eventually
end
registerInit(createWindows)

-- Register the player functions
function createNavigation()
	Epiar.RegisterKey(SDLK_UP, KEYPRESSED, "PLAYER:Accelerate()" )
	Epiar.RegisterKey(SDLK_LEFT, KEYPRESSED, "PLAYER:Rotate(30)" )
	Epiar.RegisterKey(SDLK_RIGHT, KEYPRESSED, "PLAYER:Rotate(-30)" )
	Epiar.RegisterKey(SDLK_DOWN, KEYPRESSED, "PLAYER:Rotate(PLAYER:directionTowards(PLAYER:GetMomentumAngle() + 180 ))" )
	Epiar.RegisterKey('c', KEYPRESSED, "PLAYER:Rotate(PLAYER:directionTowards(0,0))" )
	Epiar.RegisterKey(SDLK_RSHIFT, KEYPRESSED, "PLAYER:ChangeWeapon()" )
	Epiar.RegisterKey(SDLK_LSHIFT, KEYPRESSED, "PLAYER:ChangeWeapon()" )
	Epiar.RegisterKey(' ', KEYPRESSED, "PLAYER:Fire()" )

	for k =1,9 do
		kn = string.byte(k)
		ks = string.format("%d",k*1000)
		Epiar.RegisterKey(kn, KEYPRESSED, "HUD.setVisibity("..ks..")")
	end
end
registerInit(createNavigation)

function addWeapons()
	PLAYER:AddWeapon( "Minigun" )
	PLAYER:AddWeapon( "Laser" )
	PLAYER:AddWeapon( "Strong Laser" )
	PLAYER:AddWeapon( "Slow Missile" )
	PLAYER:AddAmmo( "Slow Missile",10 )
	PLAYER:AddWeapon( "Missile" )
	PLAYER:AddAmmo( "Missile",100 )
end
registerInit(addWeapons)

function coordinateToQuadrant(x,y)
	qsize = 4096
	function c2q(z)
		return math.floor( (z+qsize)/(2*qsize))
	end
	return c2q(x),c2q(y)
end

function createHUD()
	-- Location Status Bars
	x,y = PLAYER:GetPosition()
	qx,qy = coordinateToQuadrant(x,y)
	pos = HUD.newStatus("Coordinate:",130,1,string.format("( %d , %d )",x,y))
	quad = HUD.newStatus("Quadrant:",130,1,string.format("( %d , %d )",qx,qy))

	-- Weapon and Armor Status Bars
	hull = HUD.newStatus("HULL:",100,0,1.0)
	weapons = {}
	weaponsAndAmmo = PLAYER:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		if 0==ammo then ammo="---" end
		weapons[weapon] = HUD.newStatus(weapon..":",130,0,"[ ".. ammo .." ]")
	end
end
registerInit(createHUD)

updateHUD = function ()
	-- Update Positions
    x,y = PLAYER:GetPosition()
	qx,qy = coordinateToQuadrant(x,y)
	pos:setStatus(string.format("( %d , %d )",x,y))
	quad:setStatus(string.format("( %d , %d )",qx,qy))

	-- Update Weapons and Armor
	hull:setStatus(PLAYER:GetHull())
	weaponsAndAmmo = PLAYER:GetWeapons()
	cur_weapon = PLAYER:GetCurrentWeapon()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		if cur_weapon == weapon then star=" ARMED" else star="" end
		if 0==ammo then ammo="---" end
		weapons[weapon]:setStatus("[ ".. ammo .." ]".. star)
	end
end
registerPostStep(updateHUD)

