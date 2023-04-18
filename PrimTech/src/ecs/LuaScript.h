#pragma once
#include "Component.h"

namespace pt
{
	class LuaScript : public Component
	{
	public:
		LuaScript(EntIdType id) : Component(id) {}
		LuaScript(const LuaScript& other);
		~LuaScript();

		void LoadScript(const char* scriptFile);
		
		void SetBuffer(char* buffer, int size);

		void Execute(const char* funcName);

		int GetBufferSize() const { return m_size; };
		char* GetBufferP() const { return m_pbuffer; };;
		static void SetLuaState(lua_State* pL);
	private:
		static lua_State* L;
		char* m_pbuffer = nullptr;;
		int m_size = -1;
	};
}