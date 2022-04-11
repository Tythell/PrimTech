#include "StringHelper.h"

std::wstring StringHelper::StringToW(std::string str)
{
    return std::wstring(str.begin(), str.end());
}

std::string StringHelper::GetExtension(std::string filename)
{
    size_t offset = filename.find_last_of('.');
    return std::string(filename.substr(offset + 1));
}
