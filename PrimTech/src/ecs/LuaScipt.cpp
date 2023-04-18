#include "pch.h"
#include "LuaScript.h"

namespace pt
{
	lua_State* LuaScript::L = nullptr;

	LuaScript::LuaScript(const LuaScript& other) : Component(other.EntId())
	{
		m_size = other.m_size;
		this->L = other.L;

		m_pbuffer = new char[m_size];

		memcpy(m_pbuffer, other.m_pbuffer, m_size);
	}

	LuaScript::~LuaScript()
	{
		if (m_pbuffer)
		{
			delete[] m_pbuffer;
			m_pbuffer = nullptr;
		}
	}

	struct LuaDumpData
	{
		int size = 0;
		char* data = nullptr;
	};

	int WriteFunc(lua_State* L, const void* p, size_t sz, void* ud)
	{
		LuaDumpData* data = (LuaDumpData*)ud;
		char* newData = (char*)realloc(data->data, data->size + sz);

		if (newData)
		{
			memcpy(newData + data->size, p, sz);
			data->size += sz;
			data->data = newData;
		}
		else
		{
			free(newData);

			return 1;
		}
		return 0;
	}

	void LuaScript::LoadScript(const char* scriptFile)
	{
		if (m_pbuffer) delete[] m_pbuffer;

		luaL_loadfile(L, scriptFile);

		LuaDumpData dumpData;
		lua_dump(L, WriteFunc, &dumpData, 1);

		m_pbuffer = dumpData.data;
		m_size = dumpData.size;
	}

	void LuaScript::SetBuffer(char* buffer, int size)
	{
		m_size = size;
		m_pbuffer = buffer;
	}
	void LuaScript::Execute(const char* funcName)
	{
		luaL_loadbuffer(L, m_pbuffer, m_size, funcName);
		lua_pcall(L, 0, 0, 0);
		int d = lua_getglobal(L, funcName);
		lua_pcall(L, 0, 0, 0);
	}
	void LuaScript::SetLuaState(lua_State* pL)
	{
		L = pL;
	}
}