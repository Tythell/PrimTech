#pragma once
#include <Windows.h>
#include <iostream>

namespace comlib
{
	class Mutex
	{
	private:
		HANDLE mutexHandle = NULL;

	public:
		Mutex(LPCWSTR mutexName);
		~Mutex();

		void Lock();
		void Unlock();
	};
}