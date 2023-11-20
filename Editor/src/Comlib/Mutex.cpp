#include "Mutex.h"

namespace comlib
{

	Mutex::Mutex(LPCWSTR mutexName)
	{
	}

	Mutex::~Mutex()
	{
		CloseHandle(mutexHandle);
	}

	void Mutex::Lock()
	{
		WaitForSingleObject(this->mutexHandle, INFINITE);
	}

	void Mutex::Unlock()
	{
		ReleaseMutex(this->mutexHandle);
	}
}