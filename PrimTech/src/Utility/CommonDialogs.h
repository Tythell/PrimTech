#pragma once
#include <string>
#include <Windows.h>
#include <commdlg.h>
//#include <shobjidl_core.h>

class Dialogs
{
public:
	static void SetOwnerHandle(HWND* h);
	static std::string OpenFile(const char* filter, const char* subDir = "", int nroffilters = 1);
	static std::string SaveFile(const char* filter, const char* subDir = "");
private:
	static HWND* m_ownerhandle;
};