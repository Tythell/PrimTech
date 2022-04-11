#pragma once
#include <string>
class StringHelper
{
public:
	static std::wstring StringToW(std::string str);
	static std::string GetExtension(std::string filename);
};

