#pragma once
#include<comdef.h>
#include "StringHelper.h"

#define COM_ERROR(hr, msg) if(FAILED(hr) )throw COMException(hr, msg, __FILE__, __FUNCTION__, __LINE__)
//#define POPUP_ERROR(b, msg) if(!b )throw COMException(b, msg, __FILE__, __FUNCTION__, __LINE__)

class COMException
{
public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		m_whatMsg = L"Error:" + StringHelper::StringToW(msg) + L"\n";
		m_whatMsg += error.ErrorMessage();
		m_whatMsg += L"\nFile: " + StringHelper::StringToW(file);
		m_whatMsg += L"\nFunction: " + StringHelper::StringToW(function);
		m_whatMsg += L"Line: " + std::to_wstring(line);
		MessageBoxW(NULL, m_whatMsg.c_str(), L"COM_ERROR", MB_ICONERROR);
	}
	//COMException(bool b, const std::string& msg, const std::string& file, const std::string& function, int line)
	//{
	//	m_whatMsg = L"Error: " + StringHelper::StringToW(msg) + L"\n";
	//	m_whatMsg += L"\nFile: " + StringHelper::StringToW(file);
	//	m_whatMsg += L"\nFunction: " + StringHelper::StringToW(function);
	//	m_whatMsg += L"Line: " + std::to_wstring(line);
	//	MessageBoxW(NULL, m_whatMsg.c_str(), L"COM_ERROR", MB_ICONERROR);
	//}
	const wchar_t* what() const
	{
		return m_whatMsg.c_str();
	}
private:
	std::wstring m_whatMsg;
};