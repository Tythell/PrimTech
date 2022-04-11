#pragma once
#include "COMException.h"
class Popup
{
public:
	static void Error(std::string err = "Error not specified");
	static void Error(COMException& exception);
};