/**\file			audio_lua.h
 * \author			Maoserr
 * \date			Created: Sunday, March 14, 2010
 * \date			Modified: Sunday, March 14, 2010
 * \brief			Lua bridge for Audio
 * \details
 */


#ifndef __H_AUDIO_LUA
#define __H_AUDIO_LUA

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#define EPIAR_AUDIO_TABLE "Epiar.Audio"
#define EPIAR_AUDIO "Audio"

class Audio_Lua {
	public:
		// Functions to communicate with Lua
		static void RegisterAudio(lua_State *L);

		static int setSoundVolume(lua_State *L);
		static int setMusicVolume(lua_State *L);

	private:
};
#endif /*__H_AUDIO_LUA*/
