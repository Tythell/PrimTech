#include "CommonDialogs.h"

HWND* Dialogs::m_ownerhandle;

void Dialogs::SetOwnerHandle(HWND* h)
{
	m_ownerhandle = h;
}

std::string Dialogs::OpenFile(const char* filter, const char* initDir, int nrofFilters)
{
	std::string initdir = std::string(initDir);
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = *m_ownerhandle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = nrofFilters;
	ofn.lpstrInitialDir = initdir.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
	if (GetOpenFileNameA(&ofn))
		return ofn.lpstrFile;
	return std::string();
}

std::string Dialogs::SaveFile(const char* filter, const char* initDir)
{
	std::string initdir = std::string(initDir);
	OPENFILENAMEA ofn;
	char szFile[260] = { };
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = *m_ownerhandle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = initdir.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
	if (GetSaveFileNameA(&ofn))
		return ofn.lpstrFile;
	

	return std::string();
}
