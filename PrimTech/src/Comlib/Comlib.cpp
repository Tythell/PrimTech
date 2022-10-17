#include "Comlib.h"

RingBuffer::RingBuffer(const std::wstring& sharedMemName, TYPE type, const size_t& bufferSize)
	:m_type(type), m_bufferSize(bufferSize), M_OFFSET(sizeof(int) * 3), m_mutex(L"MutexMap")
{
	m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		(DWORD)0, bufferSize, sharedMemName.c_str());

	if (m_hFileMap == NULL) throw; // FATAL ERROR

	m_data = MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	m_head = (int*)m_data;
	m_tail = (int*)m_data + 1;

	if (!(GetLastError() == ERROR_ALREADY_EXISTS))
	{
		*m_head = 0;
		*m_tail = 0;
	}
}

RingBuffer::~RingBuffer()
{
	//print("Ringbuffer closed");
	UnmapViewOfFile((LPCVOID)m_data);
	CloseHandle(m_hFileMap);
}

void*& RingBuffer::GetMem()
{
	return m_data;
}

const std::string RingBuffer::GetWholeBuffer() const
{
	//std::string result = "???? ???? ";
	//int buffersize = 8;

	//char* dd = new char[1];

	//int stringSize = *m_msgLength;

	//for (int i = 0; i < m_bufferSize / 2; i+= *m_msgLength)
	//{
	//	memcpy(dd,
	//	M_OFFSET + (char*)m_data + i,
	//		*m_msgLength);

	//	result += dd;

	//	//result += std::to_string(stringSize);
	//}
	//result += "\nSize of buffer:" + std::to_string(buffersize);
	//result += "\nHead: " + std::to_string(*m_head) + " Tail: " + std::to_string(*m_tail);


	//
	//delete dd;
	return std::string("whole buffer");
}

const bool RingBuffer::IsBufferFull() const
{
	if (*m_head == *m_tail)
		return true;
	return false;
}

bool RingBuffer::Send(const char* msg, SectionHeader* size)
{
	if (m_type == READ)
	{
		printf("Error: Reader can't send messages\n");
		return false;
	}
	m_mutex.Lock();

	int paddedSize = size->msgLen + (size->msgLen % MULTIBLE_VAR);

	if (M_OFFSET + *m_head + paddedSize > m_bufferSize / 2)
	{
		int dummy = -1;
		memcpy((char*)m_data + *m_head + M_OFFSET, (int*)&dummy, sizeof(int));
		*m_head = 0;
	}

	memcpy((char*)m_data + *m_head + M_OFFSET, (int*)&paddedSize, sizeof(int));
	memcpy((char*)m_data + *m_head + M_OFFSET + sizeof(int), msg, paddedSize); // send string

	*m_head += std::abs(paddedSize) + sizeof(int);
	m_mutex.Unlock();

	return true;
}

bool RingBuffer::Recieve(char*& s, SectionHeader*& msgHeader)
{
	if (m_type == WRITE)
	{
		printf("Error: Writer can't read messages\n");
		return false;
	}

	if (IsBufferFull()) return false;

	m_mutex.Lock();

	memcpy((char*)msgHeader, M_OFFSET + (char*)m_data + *m_tail, sizeof(SectionHeader));

	if (msgHeader->msgLen < 0)
	{
		*m_tail = 0;
		memcpy((char*)msgHeader, M_OFFSET + (char*)m_data + *m_tail, sizeof(int));
	}

	if (M_OFFSET + *m_tail + msgHeader->msgLen > m_bufferSize)
		*m_tail = 0;

	memcpy((char*)s, M_OFFSET + (char*)m_data + *m_tail + sizeof(SectionHeader), msgHeader->msgLen);

	*m_tail += msgHeader->msgLen + sizeof(int);
	m_mutex.Unlock();
	return true;
}

Mutex::Mutex(LPCWSTR mutexName)
{
	m_mutexHandle = CreateMutex(nullptr, false, mutexName);
	if (!m_mutexHandle)
		std::cout << "Failed to create mutex object\n";
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		std::cout << "Mutex already exists\n";
}

Mutex::~Mutex()
{
	CloseHandle(m_mutexHandle);
}

void Mutex::Lock()
{
	WaitForSingleObject(m_mutexHandle, INFINITE);
}

void Mutex::Unlock()
{
	ReleaseMutex(m_mutexHandle);
}