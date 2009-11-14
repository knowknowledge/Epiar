#include <iostream>

#include "src/lua.h"
#include "src/lualib.h"
#include "src/lauxlib.h"

using namespace std;

lua_State *lua_state = NULL;

class Sprite {
	public:
		Sprite(double x, double y, double dir, double speed) {
			this->x = x;
			this->y = y;
			this->dir = dir;
			this->speed = speed;
		}

		void setPosition(double x, double y) {
			this->x = x;
			this->y = y;
		}

		void Debug() {
			cout << "I'm a sprite with (x, y, dir, speed) of (" << x << ", " << y << ", " << dir << ", " << speed << ")" << endl;
		}

	private:
		double x, y, dir, speed;
};

void registerSprite(lua_State *L);

void get_lua_error(lua_State *L) {
	const char *msg = lua_tostring(L, -1);
	if(msg == NULL) fprintf(stderr, "Error with no message.\n");
	else fprintf(stderr, "Lua error: %s\n", msg);
}

int main(int argc, char **argv) {

	// init lua
	lua_state = luaL_newstate();
	luaL_openlibs( lua_state );
		
	if( !lua_state ) {
		cout << "Could not initialize Lua state." << endl;
		return( -1 );
	}

	luaL_openlibs( lua_state );

	registerSprite( lua_state );

	int ret = luaL_dofile( lua_state, "script.lua" );
	if(ret != 0) {
		cout << "error running script" << endl;
		get_lua_error( lua_state );

	}


	return(0);
}

static int newSprite(lua_State *L) {
	int n = lua_gettop(L); // argc
	if(n != 4)
		return luaL_error(L, "Got %d arguments, expected 4", n);
	
	// allocate memory for a pointer to object
	Sprite **s = (Sprite **)lua_newuserdata(L, sizeof(Sprite *));

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double dir = luaL_checknumber(L, 3);
	double speed = luaL_checknumber(L, 4);

	*s = new Sprite(x, y, dir, speed);

	lua_getglobal(L, "Sprite"); // use global table "Sprite" as metatable
	lua_setmetatable(L, -2);

	return 1;
}

Sprite *checkSprite(lua_State *L, int index) {
	void *ud = 0;

	ud = (void *)lua_topointer(L, index);

	return *((Sprite **)ud);
}

static int setPosition(lua_State *L) {
	int n = lua_gettop(L); // argc

	if(n != 3)
		return luaL_error(L, "Got %d arguments, expected 2", n);

	Sprite *s = checkSprite(L, 1);

	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);

	s->setPosition(x, y);

	return 1;
}

static int render(lua_State *L) {
	int n = lua_gettop(L); // argc
	if(n != 1)
		return luaL_error(L, "Got %d arguments, expected 0", n);

	Sprite *s = checkSprite(L, 1);
	s->Debug();
	
	return 1;
}

static const luaL_Reg gSpriteFuncs[] = {
	{"new", newSprite},
	{"setPosition", setPosition},
	{"render", render},
	{NULL, NULL}
};

void registerSprite(lua_State *L) {
	luaL_register(L, "Sprite", gSpriteFuncs);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
}

