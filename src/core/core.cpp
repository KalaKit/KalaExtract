//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>
#include <fstream>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/file_utils.hpp"

#include "core/core.hpp"

using namespace KalaHeaders;

using std::cin;
using std::string_view;
using std::ostringstream;

constexpr string_view EXE_VERSION_NUMBER = "1.0";

static void Exit()
{
	ostringstream out{};
	out << "\n==========================================================================================\n";
	Log::Print(out.str());

	Log::Print("Press 'Enter' to exit...");
	cin.get();
	quick_exit(0);
}

namespace KalaExtract
{
	void Core::Run(int argc, char* argv[])
	{
		ostringstream banner{};

		banner << "\n==========================================================================================\n\n"
		       << "/$$   /$$           /$$           /$$$$$$$$             /$$                                    /$$      \n"
			   << "| $$  /$$/          | $$          | $$_____/            | $$                                   | $$     \n"
			   << "| $$ /$$/   /$$$$$$ | $$  /$$$$$$ | $$       /$$   /$$ /$$$$$$    /$$$$$$  /$$$$$$   /$$$$$$$ /$$$$$$   \n"
			   << "| $$$$$/   |____  $$| $$ |____  $$| $$$$$   |  $$ /$$/|_  $$_/   /$$__  $$|____  $$ /$$_____/|_  $$_/   \n"
			   << "| $$  $$    /$$$$$$$| $$  /$$$$$$$| $$__/    \\  $$$$/   | $$    | $$  \\__/ /$$$$$$$| $$        | $$     \n"
			   << "| $$\\  $$  /$$__  $$| $$ /$$__  $$| $$        >$$  $$   | $$ /$$| $$      /$$__  $$| $$        | $$ /$$ \n"
			   << "| $$ \\  $$|  $$$$$$$| $$|  $$$$$$$| $$$$$$$$ /$$/\\  $$  |  $$$$/| $$     |  $$$$$$$|  $$$$$$$  |  $$$$/ \n"
			   << "|__/  \\__/ \\_______/|__/ \\_______/|________/|__/  \\__/   \\___/  |__/      \\_______/ \\_______/   \\___/   \n";

		Log::Print(banner.str());

		ostringstream details{};

		details << "     | exe version: " << EXE_VERSION_NUMBER.data() << "\n";
		details << "\n==========================================================================================\n";
		Log::Print(details.str());

		Exit();
	}
}