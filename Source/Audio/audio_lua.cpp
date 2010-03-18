/**\file			audio_lua.cpp
 * \author			Maoserr
 * \date			Created: Sunday, March 14, 2010
 * \date			Modified: Sunday, March 14, 2010
 * \brief			Lua bridge for Audio
 * \details
 */

#include "includes.h"
#include "audio_lua.h"
#include "audio.h"

/**\class Audio_Lua
 * \brief Lua bridge fro UI. */

/**\brief Register's C++ functions to be callable in Lua.
 */
void Audio_Lua::RegisterAudio(lua_State *L){
	// Call them like:
	// win = Audio.setSoundVolume( ... )
	static const luaL_Reg auFunctions[] = {
		// Creation
		{"setSoundVolume", &Audio_Lua::setSoundVolume},
		{"setMusicVolume", &Audio_Lua::setMusicVolume},
		{NULL, NULL}
	};

	luaL_newmetatable(L, EPIAR_AUDIO);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */

	luaL_openlib(L, EPIAR_AUDIO, auFunctions, 0);
}

int Audio_Lua::setSoundVolume(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1 )
		return luaL_error(L, "Got %d arguments expected 1", n);

	float x = TO_FLOAT(luaL_checknumber (L, 1));

	Audio::Instance().SetSoundVol( x );
	return 1;
}

int Audio_Lua::setMusicVolume(lua_State *L){
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1 )
		return luaL_error(L, "Got %d arguments expected 1", n);

	float x = TO_FLOAT(luaL_checknumber (L, 1));

	Audio::Instance().SetMusicVol( x );
	return 1;
}