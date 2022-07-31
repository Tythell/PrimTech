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
	{
		int g(ofn.nFileOffset);
		return ofn.lpstrFile;
	}
	return std::string();
}

std::vector<std::string> CharsToStringsVector(const char* fullstring, const int& sizeofString)
{
	std::vector<std::string> v;
	int begin = 0;
	for (int i = 0; i < sizeofString; i++)
	{
		if (fullstring[i] == '\0')
		{
			char daWord[64]{'\0'};
			memcpy(daWord, fullstring + begin, i - begin);
			begin = i + 1;
			v.push_back(std::string(daWord));
			if (fullstring[i + 1] == '\0')
				break;
		}
	}
	return v;
}

std::vector<std::string> Dialogs::OpenMultifile(const char* filter, const char* subDir, int nroffilters)
{
	std::string initdir = std::string(subDir);
	OPENFILENAMEA ofn;
	const int stringSize = 1000;
	char szFile[stringSize] = {0};
	//std::string szFile;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = *m_ownerhandle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = nroffilters;
	ofn.lpstrInitialDir = initdir.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT
		| OFN_ALLOWMULTISELECT  | OFN_EXPLORER  ;
	if (GetOpenFileNameA(&ofn))
		return CharsToStringsVector(szFile, stringSize);
	return std::vector<std::string>();
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
