#pragma once
#include <string>
#include <Windows.h>
#include <commdlg.h>
//#include <shobjidl_core.h>

class Dialogs
{
public:
	static void SetOwnerHandle(HWND* h);
	static std::string OpenFile(const char* filter);
	static std::string Saveile(const char* filter);
private:
	static HWND* m_ownerhandle;
};