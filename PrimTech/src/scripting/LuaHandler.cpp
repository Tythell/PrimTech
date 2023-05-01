#include "pch.h"
#include "LuaHandler.h"

namespace PrimtTech
{
	LuaEngine::LuaEngine()
	{
		this->L = luaL_newstate();
		luaL_openlibs(L);
	}

	LuaEngine::~LuaEngine()
	{
		lua_close(L);
	}

	int LuaEngine::InitCompType(std::string typeStr, PrimtTech::HasComponent compType, int typeTableIdx)
	{
		lua_pushinteger(L, (int)compType);
		lua_setfield(L, typeTableIdx, typeStr.c_str());

		lua_newtable(L);
		int tableIdx = lua_gettop(L);
		lua_pushvalue(L, tableIdx);
		lua_setglobal(L, typeStr.c_str());
		//lua_pushcfunction(L, Lua_SetImage);
		//lua_setfield(L, -2, "set");

		typeStr += "MetaTable";

		luaL_newmetatable(L, typeStr.c_str());
		lua_pushstring(L, "__index");
		lua_pushvalue(L, tableIdx);
		lua_settable(L, -3);


		return tableIdx;
	}

	int LuaEngine::CreateLuaCompTable()
	{
		lua_newtable(L);
		int typeIdx = lua_gettop(L);
		lua_pushvalue(L, typeIdx);
		lua_setglobal(L, "Type");
		return typeIdx;
	}

	int LuaEngine::InitEntTable()
	{
		lua_newtable(L);
		int entIdx = lua_gettop(L);
		lua_pushvalue(L, entIdx);
		lua_setglobal(L, "Ent");
		lua_pushcfunction(L, Lua_AddComp);
		lua_setfield(L, -2, "AddComponent");
		lua_pushcfunction(L, Lua_GetComp);
		lua_setfield(L, -2, "GetComponent");
		lua_pushcfunction(L, Lua_FreeComp);
		lua_setfield(L, -2, "FreeComponent");
		m_entTableIndex = entIdx;
		return entIdx;
	}

	void LuaEngine::ChangeCurrentLuaEnt(pt::Entity* pEnt)
	{
		lua_pushlightuserdata(L, pEnt);
		lua_setfield(L, m_entTableIndex, "ptr");
	}

	void LuaEngine::ChangeCurrentLuaEnt(EntIdType idx)
	{
		ChangeCurrentLuaEnt(&pt::Entity::GetAllEnts()[idx]);
	}

	void LuaEngine::AddTypeFunc(lua_CFunction func, int tableIdx, std::string funcName)
	{
		lua_pushcfunction(L, func);
		lua_setfield(L, tableIdx, funcName.c_str());
	}

	int LuaEngine::Lua_AddComp(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");
		//Lua_dumpStack(L);

		int type = lua_tointeger(L, 1);

		pt::Entity* pEnt = (pt::Entity*)lua_touserdata(L, -1);

		switch (type)
		{
		case ec_transform:
		{
			lua_pushlightuserdata(L, &pEnt->Transform());

			int d = luaL_getmetatable(L, "TransformMetaTable");
			assert(lua_istable(L, -1));
			lua_setmetatable(L, -2);

			break;
		}
		case PrimtTech::ec_meshRef:
		{
			// args (string, int)

			const char* name = lua_tostring(L, 2);

			pt::MeshRef* pImag = pEnt->AddComponent<pt::MeshRef>();
			pImag->Init(name);

			lua_pushlightuserdata(L, pImag);

			luaL_getmetatable(L, "MeshMetaTable");
			assert(lua_istable(L, -1));
			lua_setmetatable(L, -2);
			break;
		}
		case PrimtTech::ec_cam:
		{
			// args (string, int)

			const char* name = lua_tostring(L, 2);

			pt::Camera* pImag = pEnt->AddComponent<pt::Camera>();

			lua_pushlightuserdata(L, pImag);

			luaL_getmetatable(L, "CameraMetaTable");
			assert(lua_istable(L, -1));
			lua_setmetatable(L, -2);
			break;
		}
		default:
			break;
		}

		return 1;
	}

	int LuaEngine::Lua_GetComp(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");

		int type = lua_tointeger(L, 1);

		pt::Entity* pEnt = (pt::Entity*)lua_touserdata(L, -1);

		switch (type)
		{
		case ec_transform:
		{
			lua_pushlightuserdata(L, &pEnt->Transform());

			luaL_getmetatable(L, "TransformMetaTable");
			assert(lua_istable(L, -1));
			lua_setmetatable(L, -2);
			break;
		}
		case PrimtTech::ec_meshRef:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");

			break;
		}
		case PrimtTech::ec_cam:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");

			break;
		}
		case PrimtTech::ec_light:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");

			break;
		}
		case PrimtTech::ec_rigidBodies:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");
			break;
		}
		default:
			THROW_POPUP_ERROR(false, "Not implemented yet");
			break;
		}
		return 1;
	}

	int LuaEngine::Lua_FreeComp(lua_State* L)
	{
		return 1;
	}

	lua_State* LuaEngine::GetLuaState()
	{
		return this->L;
	}
}

