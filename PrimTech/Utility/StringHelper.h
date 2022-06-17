#pragma once
#include <string>
class StringHelper
{
public:
	static const std::wstring StringToW(std::string str);
	static std::string GetExtension(std::string filename);
	static std::string GetName(std::string path);
};

