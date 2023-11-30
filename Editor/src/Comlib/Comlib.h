#pragma once
#include <string>
#include "Memory.h"
#include "Mutex.h"
#include "MsgHeaders.h"

namespace comlib
{
	enum ProcessType { Producer, Consumer };

	class RingBuffer
	{
	private:
		Mutex* m_mutex;
		Memory* m_sharedMemory;
		char* m_messageData;

		size_t* m_head;
		size_t* m_tail;
		size_t* m_freeMemory;

		ControlHeader* m_ctrler = nullptr;
		ProcessType m_type;
	public:
		RingBuffer(LPCWSTR bufferName, size_t bufferSize, ProcessType type);
		~RingBuffer();

		Memory* GetSharedMemory() { return m_sharedMemory; }
		bool Send(char* message, SectionHeader* secHeader);
		bool Recieve(char*& message, SectionHeader*& msgHeader);
	};
}



