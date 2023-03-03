#include"pch.h"
#include "StringHelper.h"

const std::wstring StringHelper::StringToW(std::string str)
{
    return std::wstring(str.begin(), str.end());
}

std::string StringHelper::GetExtension(std::string filename)
{
    size_t offset = filename.find_last_of('.');
    return std::string(filename.substr(offset + 1));
}

std::string StringHelper::GetName(std::string path)
{
    size_t offset = path.find_last_of("/");
    size_t offset2 = path.find_last_of("\\"); 
    return std::string(path.substr(offset + offset2 + 2));
}
