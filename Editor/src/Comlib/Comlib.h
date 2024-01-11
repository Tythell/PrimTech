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
		Mutex* m_mutex = nullptr;
		Memory* m_sharedMemory = nullptr;
		char* m_messageData = nullptr;

		size_t* m_head = nullptr;
		size_t* m_tail = nullptr;
		size_t* m_freeMemory = nullptr;

		ControlHeader* m_ctrler = nullptr;
		ProcessType m_type = Producer;
	public:
		RingBuffer(LPCWSTR bufferName, size_t bufferSize, ProcessType type);
		~RingBuffer();

		Memory* GetSharedMemory() { return m_sharedMemory; }
		bool Send(char* message, SectionHeader* secHeader);
		bool Recieve(char*& message, SectionHeader*& msgHeader);
	};
}



