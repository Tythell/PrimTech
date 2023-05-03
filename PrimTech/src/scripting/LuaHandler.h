#pragma once
#include <lua/lua.hpp>
#include<string>
#include "ecs/Entity.h"
#include "../Input/Keyboard.h"

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
		int UpdateDeltaTime(float dt);
		void ChangeCurrentLuaEnt(pt::Entity* pEnt);
		void ChangeCurrentLuaEnt(EntIdType idx);
		void AddTypeFunc(lua_CFunction func, int tableIdx, std::string funcName);
		bool ExecuteFunction(const char* funcName, char* scriptBuffer, int size);

		static int Lua_IsKeyPress(lua_State* L);

		lua_State* GetLuaState();

	private:
		lua_State* L = nullptr;

		int m_entTableIndex = 0;
	};
}