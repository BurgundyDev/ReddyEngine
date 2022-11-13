extern "C" {
	#include <../lua/lua.h>
	#include <../lua/lauxlib.h>
	#include <../lua/lualib.h>
}

#include "Engine/Event.h"
#include "Engine/LuaBindings.h"
#include "Engine/ReddyEngine.h"

namespace Engine
{
    LuaEvent::~LuaEvent()
    {
        if (!dataLuaName.empty())
        {
            auto L = getLuaBindings()->getState();

            // We need to remove the reference to the event data so it gets garbage collected
            lua_getglobal(L, "EVTS_t");
            lua_pushnil(L);
            lua_setfield(L, -2, dataLuaName.c_str());
        }
    }
}
