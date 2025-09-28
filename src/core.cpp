//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>
#include <fstream>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/string_utils.hpp"
#include "KalaHeaders/file_utils.hpp"

#include "core.hpp"
#include "command.hpp"

using namespace KalaHeaders;

using KalaExtract::Core;
using KalaExtract::Command;

using std::cin;
using std::getline;
using std::string_view;
using std::ostringstream;
using std::min;

constexpr string_view EXE_VERSION_NUMBER = "1.0";

static void PrintBanner()
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

	details << "     | cli version: " << EXE_VERSION_NUMBER.data() << "\n";
	details << "\n==========================================================================================\n";
	Log::Print(details.str());
}

static void GetParams(int argc, char* argv[]);
static void WaitForInput();
static void Exit();

namespace KalaExtract
{
	void Core::Run(int argc, char* argv[])
	{
		PrintBanner();
		GetParams(argc, argv);
		WaitForInput();

		Exit();
	}
}

void GetParams(int argc, char* argv[])
{
	if (argc == 1) WaitForInput();

	vector<string> params{};
	string insertedCommand{};

	bool shouldExit = (string(argv[argc - 1]) == "--exit");

	for (int i = 1; i < argc; ++i)
	{
		if (shouldExit
			&& i == argc - 1)
		{
			break; //skip the last param if it is --exit
		}

		params.emplace_back(argv[i]);
		insertedCommand += "'" + string(argv[i]) + "' ";
	}

	if (params.empty()) WaitForInput();

	Log::Print(
		"Inserted command: " + insertedCommand + "\n",
		"PARSE",
		LogType::LOG_INFO);

	Command::ParseCommand(params);

	if (shouldExit) Exit();
}

void WaitForInput()
{
	string line{};

	while (true)
	{
		Log::Print("Enter command:");

		getline(cin, line);

#ifdef _WIN32
		system("cls");
#else
		system("clear");
#endif

		PrintBanner();

		vector<string> splitValue = SplitString(line, " ");

		if (splitValue.size() == 0)
		{
			Log::Print(
				"No commands were passed. Type '--help' to list all commands",
				"PARSE",
				LogType::LOG_INFO);

			continue;
		}

		if (splitValue[0] == "--exit")
		{
			Log::Print(
				"KalaExtract was shut down by user.",
				"PARSE",
				LogType::LOG_INFO);

			Exit();
		}

		bool shouldExit = (
			!splitValue.empty()
			&& splitValue.back() == "--exit");

		if (shouldExit) splitValue.pop_back();

		Command::ParseCommand(splitValue);

		if (shouldExit) Exit();
	}
}

void Exit()
{
	ostringstream out{};
	out << "\n==========================================================================================\n";
	Log::Print(out.str());

	Log::Print("Press 'Enter' to exit...");
	cin.get();
	quick_exit(0);
}