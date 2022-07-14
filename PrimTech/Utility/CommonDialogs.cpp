#include "CommonDialogs.h"

HWND* Dialogs::m_ownerhandle;

void Dialogs::SetOwnerHandle(HWND* h)
{
	m_ownerhandle = h;
}

std::string Dialogs::OpenFile(const char* filter)
{
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = *m_ownerhandle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileNameA(&ofn))
		return ofn.lpstrFile;
	return std::string();
}

std::string Dialogs::Saveile(const char* filter)
{
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = *m_ownerhandle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetSaveFileNameA(&ofn))
		return ofn.lpstrFile;
	return std::string();
	return std::string();
}
