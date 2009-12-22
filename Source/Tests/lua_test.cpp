#include "includes.h"
#include "common.h"
#include "Debug/lua_test.h"
#include "Utilities/lua.h"

void lua_test(void) {
	LuaScript *script = new LuaScript("test.lua");

	char buff[256] = {0};
	int error = 0;

	while(fgets(buff, sizeof(buff), stdin) != NULL) {
		error = luaL_loadbuffer(LuaScript::luaVM, buff, strlen(buff), "line") || lua_pcall(LuaScript::luaVM, 0, 0, 0);
		if(error) {
			fprintf(stderr, "%s", lua_tostring(LuaScript::luaVM, -1));
			lua_pop(LuaScript::luaVM, 1); // pop error message from the stack
		}
	}

}

