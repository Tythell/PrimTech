#pragma once
#include <Windows.h>
#include <iostream>

namespace comlib
{
	class Mutex
	{
	private:
		HANDLE mutexHandle;

	public:
		Mutex(LPCWSTR mutexName);
		~Mutex();

		void Lock();
		void Unlock();
	};
}