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

	bool LuaScript::LoadScript(const char* scriptFile)
	{
		lua_State* loadState = luaL_newstate();
		int d = luaL_loadfile(loadState, scriptFile);

		if (d == LUA_OK)
		{
			m_fileName = StringHelper::GetName(scriptFile);
			LuaDumpData dumpData;
			lua_dump(loadState, WriteFunc, &dumpData, 1);

			if (m_pbuffer) delete[] m_pbuffer;

			m_pbuffer = dumpData.data;
			m_size = dumpData.size;
		}

		lua_close(loadState);
		return d == LUA_OK;
	}

	void LuaScript::SetBuffer(char* buffer, int size)
	{
		m_size = size;
		m_pbuffer = buffer;
	}
	bool LuaScript::Execute(const char* funcName)
	{
		if (m_pbuffer)
		{
			luaL_loadbuffer(L, m_pbuffer, m_size, funcName);
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
	void LuaScript::SetLuaState(lua_State* pL)
	{
		L = pL;
	}
}