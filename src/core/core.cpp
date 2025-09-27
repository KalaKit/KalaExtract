//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

#include "KalaHeaders/log_utils.hpp"

#include "core/core.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using std::cin;

namespace KalaExtract
{
	void Core::Run(int argc, char* argv[])
	{
		Log::Print("start");

		cin.get();
	}
}