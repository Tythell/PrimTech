#pragma once

#include <Windows.h>
#include <iostream>

namespace comlib
{

	struct ControlHeader
	{
		size_t head = 0;
		size_t tail = 0;
		size_t freeMemory = 0;
	};

	class Memory
	{
	private:
		HANDLE memoryFilemap = NULL;
		HANDLE controlFilemap = NULL;

		char* memoryData = NULL;
		size_t* controlData = NULL;

		size_t bufferSize = NULL;
		size_t controlbufferSize = NULL;

		LPCWSTR bufferName = NULL;
		LPCWSTR ctrlbufferName = NULL;

	public:
		Memory(LPCWSTR bufferName, size_t bufferSize);
		~Memory();
		void InitializeFilemap(LPCWSTR buffername);
		void InitializeFileview();

		char* GetMemoryBuffer() { return memoryData; }
		size_t* GetControlBuffer() { return controlData; }

		size_t GetControlBufferSize() { return this->controlbufferSize; }
		size_t GetBufferSize() { return this->bufferSize; }
	};

}