#pragma once
#include <string>
#include <Windows.h>
#include <iostream>
#include <ctime>

#define DEFAULT_BUFFERSIZE (1 << 11)
#define DEFAULT_MESSAGESIZE 1024 * 1024
#define MULTIBLE_VAR 64
#define LENGTH_SIZE sizeof(int)

enum TYPE { WRITE, READ };

class Mutex
{
public:
	Mutex(LPCWSTR mutexName);
	~Mutex();

	void Lock();
	void Unlock();
private:
	HANDLE m_mutexHandle;
};

enum Headers
{
	MESSAGE = 0,

};

struct messageHeader
{
	char message[1024];
};

struct SectionHeader
{
	Headers header;
	UINT msgLen;
	UINT msgID;
};

class RingBuffer
{
private:
	Mutex m_mutex;
	TYPE m_type;

	HANDLE m_hFileMap;
	void* m_data;
	std::string m_memName;
	UINT m_bufferSize;

	int* m_head;
	int* m_tail;
	UINT m_count = 0;
	const UINT M_OFFSET;
	const bool IsBufferFull() const;
public:
	RingBuffer(const std::wstring& sharedMemName, TYPE type, const size_t& bufferSize = DEFAULT_BUFFERSIZE);
	~RingBuffer();

	void*& GetMem();
	bool Send(const char* msg, SectionHeader* size);
	bool Recieve(char*& s, SectionHeader*& size);
	const std::string GetWholeBuffer() const;
};

template<class T>
void print(T t)
{
	std::cout << t << std::endl;
}