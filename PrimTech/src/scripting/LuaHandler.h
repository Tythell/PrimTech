#pragma once
#include <lua/lua.hpp>
#include<string>
#include "ecs/Entity.h"

namespace PrimtTech
{
	class LuaEngine
	{
	public:
		LuaEngine();
		~LuaEngine();

		int InitCompType(std::string typeStr, PrimtTech::HasComponent compType, int typeTableIdx);
		int CreateLuaCompTable();
		int InitEntTable();
		void ChangeCurrentLuaEnt(pt::Entity* pEnt);
		void ChangeCurrentLuaEnt(EntIdType idx);
		void AddTypeFunc(lua_CFunction func, int tableIdx, std::string funcName);
		bool ExecuteFunction(const char* funcName, char* scriptBuffer, int size);

		lua_State* GetLuaState();

	private:
		lua_State* L = nullptr;

		static int Lua_AddComp(lua_State* L);
		static int Lua_GetComp(lua_State* L);
		static int Lua_FreeComp(lua_State* L);

		int m_entTableIndex = 0;
	};
}