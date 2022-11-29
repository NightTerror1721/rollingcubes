#include "natives.h"

#include "env.h"


namespace lua::utils
{
    namespace
    {
        static void dumpTable(std::ostream& os, lua_State* state, const LuaRef& ref, unsigned int identation, unsigned int identationIncrement)
        {
            unsigned int newIdentation = identation + identationIncrement;
            std::string ident = std::string(newIdentation, ' ');
            bool empty = true;

            os << "{";

            ref.push();
            luabridge::push(state, luabridge::Nil());
            while (lua_next(state, -2))
            {
                LuaRef key = LuaRef::fromStack(state, -2);
                LuaRef val = LuaRef::fromStack(state, -1);

                if (identationIncrement > 0)
                    os << std::endl << ident;
                dump(os, state, key, 0, 0);
                os << " = ";
                dump(os, state, val, newIdentation, identationIncrement);

                empty = false;

                lua_pop(state, 1);
            }
            lua_pop(state, 1);

            if (!empty)
            {
                if(identationIncrement > 0)
                    os << std::endl;
                if(identation > 0)
                    os << std::string(identation, ' ');
            }
            os << "}";
        }
    }

	void dump(std::ostream& os, lua_State* state, const LuaRef& ref, unsigned int identation, unsigned int identationIncrement)
	{
        switch (ref.type())
        {
            case LUA_TNIL:
                os << "nil";
                break;

            case LUA_TNUMBER:
                os << ref.cast<lua_Number>();
                break;

            case LUA_TBOOLEAN:
                os << (ref.cast<bool>() ? "true" : "false");
                break;

            case LUA_TSTRING:
                os << '"' << ref.cast<std::string>() << '"';
                break;

            case LUA_TTABLE:
                dumpTable(os, state, ref, identation, identationIncrement);
                break;

            case LUA_TFUNCTION:
                os << ref.tostring();
                break;

            case LUA_TUSERDATA:
                os << ref.tostring();
                break;

            case LUA_TTHREAD:
                os << ref.tostring();
                break;

            case LUA_TLIGHTUSERDATA:
                os << ref.tostring();
                break;

            default:
                os << "unknown";
                break;
        }
	}

    void dump(std::ostream& os, lua_State* state, const LuaEnv& env, unsigned int identation, unsigned int identationIncrement)
    {
        dump(os, state, env.getLuaRef(), identation, identationIncrement);
    }
}
