#pragma once
#include <string>
#include "Memory.h"
#include "Mutex.h"
#include "MahaHeaders.h"

enum ProcessType { Producer, Consumer };

class Comlib
{
private:
	Mutex* m_mutex;
	Memory* m_sharedMemory;
	char* m_messageData;

	size_t* m_head;
	size_t* m_tail;
	size_t* m_freeMemory;

	ControlHeader* m_ctrler;
	ProcessType m_type;
public:
	Comlib(LPCWSTR bufferName, size_t bufferSize, ProcessType type);
	~Comlib();

	Memory* GetSharedMemory() { return m_sharedMemory; }
	bool Send(char* message, SectionHeader* secHeader);
	bool Recieve(char*& message, SectionHeader*& msgHeader);
};