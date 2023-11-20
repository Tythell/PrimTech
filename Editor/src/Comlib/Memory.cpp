#include "Memory.h"

namespace comlib
{

	Memory::Memory(LPCWSTR bufferName, size_t bufferSize)
	{
		this->bufferSize = bufferSize * 1048576;
		this->controlbufferSize = sizeof(ControlHeader);
		this->ctrlbufferName = L"CtrlMap";

		InitializeFilemap(bufferName);
		InitializeFileview();
	}

	Memory::~Memory()
	{
		//Deleting memory close map and file view
		UnmapViewOfFile(memoryData);
		CloseHandle(memoryFilemap);

		//Deleting the new fil emap and file view
		UnmapViewOfFile(controlData);
		CloseHandle(controlFilemap);
	}

	void Memory::InitializeFilemap(LPCWSTR buffername)
	{
		//shared memory for memorybuffer
		memoryFilemap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufferSize, buffername);
		if (memoryFilemap == NULL)
			std::cout << "Failed to create file mapping object\n";
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			std::cout << "File mapping object already exists - Its shared\n";

		/*Shared memory for controlbuffer */
		controlFilemap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, controlbufferSize, ctrlbufferName);
		if (controlFilemap == NULL)
			std::cout << "Failed to create file mapping object\n";
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			std::cout << "File mapping object already exists - It's shared\n";

	}

	void Memory::InitializeFileview()
	{
		// Shared memory for memorybuffer
		memoryData = (char*)MapViewOfFile(memoryFilemap, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
		if (memoryData == NULL)
			std::cout << "View of file mapping object for memorydata failed\n";
		controlData = (size_t*)MapViewOfFile(controlFilemap, FILE_MAP_ALL_ACCESS, 0, 0, controlbufferSize);
		if (controlData == NULL)
			std::cout << "View of file mapping object for controldata failed\n";


	}

}