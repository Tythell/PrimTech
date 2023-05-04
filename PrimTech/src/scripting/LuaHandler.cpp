#include "pch.h"
#include "LuaHandler.h"

namespace PrimtTech
{
	LuaEngine::LuaEngine()
	{
		this->L = luaL_newstate();
		luaL_openlibs(L);

		lua_newtable(L);
		int keyIndex = lua_gettop(L);
		lua_pushvalue(L, keyIndex);
		lua_setglobal(L, "Input");
		lua_pushcfunction(L, Lua_IsKeyPress);
		lua_setfield(L, -2, "KeyDown");
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
		lua_pushcfunction(L, pt::Entity::Lua_AddComp);
		lua_setfield(L, -2, "AddComponent");
		lua_pushcfunction(L, pt::Entity::Lua_GetComp);
		lua_setfield(L, -2, "GetComponent");
		lua_pushcfunction(L, pt::Entity::Lua_FreeComp);
		lua_setfield(L, -2, "FreeComponent");

		lua_pushcfunction(L, pt::Entity::Lua_Move);
		lua_setfield(L, -2, "Move");
		lua_pushcfunction(L, pt::Entity::Lua_Rotate);
		lua_setfield(L, -2, "Rotate");
		lua_pushcfunction(L, pt::Entity::Lua_Scale);
		lua_setfield(L, -2, "Rotate");
		lua_pushcfunction(L, pt::Entity::Lua_SetPosition);
		lua_setfield(L, -2, "SetPosition");
		lua_pushcfunction(L, pt::Entity::Lua_SetRotation);
		lua_setfield(L, -2, "SetRotation");
		lua_pushcfunction(L, pt::Entity::Lua_SetScale);
		lua_setfield(L, -2, "SetScale");

		m_entTableIndex = entIdx;
		return entIdx;
	}

	int LuaEngine::UpdateDeltaTime(float dt)
	{
		lua_pushnumber(L, (lua_Number)dt);
		lua_setglobal(L, "DeltaTime");
		return 0;
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

	bool LuaEngine::ExecuteFunction(const char* funcName, char* scriptBuffer, int size)
	{
		if (scriptBuffer)
		{
			luaL_loadbuffer(L, scriptBuffer, size, funcName);
			lua_pcall(L, 0, 0, 0);
			int d = lua_getglobal(L, funcName);
			if (d != LUA_OK)
			{
				lua_pcall(L, 0, 0, 0);
				return true;
			}

		}
		return false;
	}

	int LuaEngine::Lua_IsKeyPress(lua_State* L)
	{
		size_t size = 1;
		const char* d = lua_tolstring(L, -1, &size);

		lua_pushinteger(L, (int)KeyboardHandler::IsKeyDown(d[0]));

		return 1;
	}

	lua_State* LuaEngine::GetLuaState()
	{
		return this->L;
	}
}

