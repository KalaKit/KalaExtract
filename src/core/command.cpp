//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <array>
#include <vector>
#include <filesystem>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/string_utils.hpp"
#include "KalaHeaders/file_utils.hpp"

#include "core/core.hpp"
#include "core/command.hpp"

using namespace KalaHeaders;

using KalaExtract::Core;

using std::string;
using std::string_view;
using std::array;
using std::vector;
using std::filesystem::path;

static const array<string_view, 9> commands =
{
	"--help",
	"--exit",
	"--create",
	"--get",
	"--replace",
	"--remove",
	"--reset",
	"--compress",
	"--decompress"
};

enum Command_Get_Range
{
	GET_ALL,
	GET_TARGET
};

enum Command_Remove_Range
{
	REMOVE_ALL,
	REMOVE_TARGET
};

enum Command_Compress_Range
{
	COMPRESS_ALL,
	COMPRESS_TARGET_WITH_NAME,
	COMPRESS_TARGET_NO_NAME
};

enum Command_Decompress_Range
{
	DECOMPRESS_ALL,
	DECOMPRESS_TARGET
};

static void Command_Help(const string& targetCommand);

static void Command_Create(const path& targetBinary);

static void Command_Get(
	Command_Get_Range range,
	const string& nameOrIndex,
	const path& targetBinary);

static void Command_Replace(
	const string& nameOrIndex,
	const path& replaceWith,
	const path& targetBinary);

static void Command_Remove(
	Command_Remove_Range range,
	const string& nameOrIndex,
	const path& targetBinary);

static void Command_Reset(const path& targetBinary);

static void Command_Compress(
	Command_Compress_Range range,
	const string& newFile,
	const string& name,
	const string& targetBinary);

static void Command_Decompress(
	Command_Decompress_Range range,
	const string& nameOrIndex,
	const path& decompressTo,
	const path& targetBinary);

namespace KalaExtract
{
	void Command::ParseCommand(const vector<string>& params)
	{
		if (find(commands.begin(), commands.end(), params[0]) == commands.end())
		{
			Log::Print(
				"Inserted command '" + params[0] + "' does not exist! Type '--help' to list all commands.",
				"PARSE",
				LogType::LOG_ERROR);

			return;
		}

		//list all commands
		if (params[0] == "--help")
		{
			if (params.size() == 1)
			{
				Command_Help("");
			}
			else if (params.size() == 2)
			{
				Command_Help(params[1]);
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--help' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}

		//create new empty target binary
		else if (params[0] == "--create")
		{
			if (params.size() == 2)
			{
				Command_Create(params[1]);
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--create' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}

		//print data about a bundle from target binary
		else if (params[0] == "--get")
		{
			if (params.size() == 3)
			{
				if (params[1] == "--all")
				{
					Command_Get(
						Command_Get_Range::GET_ALL,
						"",         //name is unused if no name is passed
						params[2]);
				}
				else
				{
					Command_Get(
						Command_Get_Range::GET_TARGET,
						params[1],
						params[2]);
				}
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--get' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}

		//replace a bundle from target binary
		else if (params[0] == "--replace")
		{
			if (params.size() == 4)
			{
				Command_Replace(
					params[1],
					params[2],
					params[3]);
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--replace' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}

		//remove a bundle from target binary
		else if (params[0] == "--remove"
			&& params.size() == 3)
		{
			if (params.size() == 3)
			{
				if (params[1] == "--all")
				{
					Command_Remove(
						Command_Remove_Range::REMOVE_ALL,
						"",         //name is unused if no name is passed
						params[2]);
				}
				else
				{
					Command_Remove(
						Command_Remove_Range::REMOVE_TARGET,
						params[1],
						params[2]);
				}
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--remove' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}

		//reset a target binary by removing all bundles and clearing the header
		else if (params[0] == "--reset")
		{
			if (params.size() == 2)
			{
				Command_Reset(params[1]);
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--reset' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}

		//copy a file inside a target binary
		else if (params[0] == "--compress")
		{
			if (params[1] == "--all"
				&& params.size() == 3)
			{
				Command_Compress(
					Command_Compress_Range::COMPRESS_ALL,
					"",         //name is unused if no name is passed
					"",         //path is unused if no path is passed
					params[2]);
			}
			else if (params.size() == 4)
			{
				Command_Compress(
					Command_Compress_Range::COMPRESS_TARGET_WITH_NAME,
					params[1],
					params[2],
					params[3]);
			}
			else if (params.size() == 3)
			{
				Command_Compress(
					Command_Compress_Range::COMPRESS_TARGET_NO_NAME,
					params[1],
					"",         //name is unused if no name is passed
					params[2]);
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--compress' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}

		//copy a bundle outside as a file from a target binary
		else if (params[0] == "--decompress")
		{
			if (params[1] == "--all"
				&& params.size() == 4)
			{
				Command_Decompress(
					Command_Decompress_Range::DECOMPRESS_ALL,
					"",         //name is unused if no name is passed
					params[2],
					params[3]);
			}
			else if (params[1] != "--all"
				&& params.size() == 4)
			{
				Command_Decompress(
					Command_Decompress_Range::DECOMPRESS_TARGET,
					params[1],
					params[2],
					params[3]);
			}
			else
			{
				Log::Print(
					"Incorrect parameter structure was passed to '--decompress' command! Type '--help' to list all commands.",
					"PARSE",
					LogType::LOG_ERROR);

				return;
			}
		}
	}
}

void Command_Help(const string& targetCommand)
{
	if (targetCommand.empty())
	{
		ostringstream oss{};

		oss << "Listing all commands. Type '--command commandname'"
			<< " and replace 'commandname' with your desired command to see info about that command\n\n";

		for (const auto& val : commands)
		{
			oss << val << "\n";
		}

		Log::Print(oss.str());

		return;
	}
	else
	{
		ostringstream oss{};

		if (find(commands.begin(), commands.end(), targetCommand) == commands.end())
		{
			Log::Print(
				"Did not find command '" + targetCommand + "'! Type '--help' to list all commands.",
				"PARSE",
				LogType::LOG_ERROR);

			return;
		}

		if (targetCommand == "--help")
		{
			oss << "Lists all commands or lists the info about the selected command"
				<< " if a command was passed as the second parameter\n\n"
				
				<< "Accepted parameters:\n\n"

				<< "[1] --help: the command itself\n"
				<< "[2] commandName: the optional command name you want more info about";

			Log::Print(oss.str());

			return;
		}
		else if (targetCommand == "--exit")
		{
			oss << "Exits KalaExtract\n\n"

				<< "Accepted parameters:\n\n"

				<< "[1] --exit: the command itself";

			Log::Print(oss.str());

			return;
		}
		else if (targetCommand == "--get")
		{
			oss << "Prints info about the selected bundle:\n"
				<< "- name\n"
				<< "- index\n"
				<< "- decompressed size\n"
				<< "- compressed size\n"
				<< "- the byte where the bundle data starts\n"
				<< "- the byte where the bundle data ends\n\n"
				
				<< "Accepted parameters:\n\n"

				<< "When getting info about all bundles:\n"
				<< "[1] --get: the command itself\n"
				<< "[2] --all: get all bundles\n"
				<< "[3] targetBinary: the target binary you want to get info from\n\n"
				
				<< "When getting info about a specific bundle:\n"
				<< "[1] --get: the command itself\n"
				<< "[2] targetBundle: the name or index of the bundle you want to get info about\n"
				<< "[3] targetBinary: the target binary you want to get info from";

			Log::Print(oss.str());

			return;
		}
		else if (targetCommand == "--replace")
		{
			oss << "Replaces one bundle with another. The old bundle is lost forever,"
				<< " so decompressing it is recommended if you want to keep it.\n\n"
				
				<< "Accepted parameters:\n\n"

				<< "[1] --replace: the command itself\n"
				<< "[2] name/index: the name or index of the bundle you want replaced\n"
				<< "[3] targetBundle: the file you want to replace the bundle with\n"
				<< "[4] targetBinary: the target binary where the bundle will be replaced at";

			Log::Print(oss.str());

			return;
		}
		else if (targetCommand == "--remove")
		{
			oss << "Removes a bundle from the target binary. The old bundle is lost forever,"
				<< " so decompressing it is recommended if you want to keep it.\n\n"
				
				<< "Accepted parameters:\n\n"

				<< "When removing all bundles:\n"
				<< "[1] --remove: the command itself\n"
				<< "[2] --all: remove all bundles\n"
				<< "[3] targetBinary: the target binary where all bundles are removed from\n\n"
				
				<< "When removing a specific bundle:\n"
				<< "[1] --remove: the command itself\n"
				<< "[2] targetBundle: the name or index of the bundle you want removed\n"
				<< "[3] targetBinary: the target binary where the specific bundle is removed from";

			Log::Print(oss.str());

			return;
		}
		else if (targetCommand == "--reset")
		{
			oss << "Removes the global header and all bundles from the target binary."
				<< " The old bundles are lost forever so decompressing them is recommended if you want to keep them\n\n"

				<< "Accepted parameters:\n\n"

				<< "[1] --reset: the command itself\n"
				<< "[2] targetBinary: the target binary which you want to reset";

			Log::Print(oss.str());

			return;
		}
		else if (targetCommand == "--compress")
		{
			oss << "Either compresses selected file or dir to '.7z'"
				<< " or simply copies the already compressed file to the target binary.\n\n"

				<< "Accepted parameters:\n\n"

				<< "When compressing all files and folders except KalaExtract as separate bundles in current directory:\n"
				<< "[1] --compress: the command itself\n"
				<< "[2] --all: get all files and folders except KalaExtract in current folder\n"
				<< "[3] targetBinary: the target binary where the bundles will be compressed/stored to\n\n"

				<< "When compressing selected file or folder with custom name:\n"
				<< "[1] --compress: the command itself\n"
				<< "[2] filePath: the path that will be compressed/stored\n"
				<< "[3] name: the name this bundle will be saved as\n"
				<< "[4] targetBinary: the target binary where the bundle will be compressed/stored to\n\n"

				<< "When compressing selected file or folder with automatic name (saves as path filename):\n"
				<< "[1] --compress: the command itself\n"
				<< "[2] filePath: the path that will be compressed/stored\n"
				<< "[3] targetBinary: the target binary where the bundle will be compressed/stored to";

			Log::Print(oss.str());

			return;
		}
		else if (targetCommand == "--decompress")
		{
			oss << "Either decompresses selected bundle or dir from '.7z'"
				<< " or simply copies the already decompressed file from the target binary.\n\n"

				<< "Accepted parameters:\n\n"

				<< "When decompressing all bundles:\n"
				<< "[1] --decompress: the command itself\n"
				<< "[2] --all: all bundles in the target binary\n"
				<< "[3] filePath: the path where all the bundles will be decompressed to\n\n"

				<< "When decompressing selected bundle:\n"
				<< "[1] --decompress: the command itself\n"
				<< "[2] targetBundle: the name or index of the bundle in the target binary\n"
				<< "[3] filePath: the path where the bundle will be decompressed to";

			Log::Print(oss.str());

			return;
		}
	}
}

void Command_Create(const path& targetBinary)
{
	string result = CreateFile(targetBinary);

	if (!result.empty())
	{
		ostringstream oss{};

		oss << "Failed to create new binary at path '" << targetBinary
			<< "'! Reason: " << result;

		Log::Print(
			oss.str(),
			"COMMAND_CREATE",
			LogType::LOG_ERROR);

		return;
	}

	Log::Print(
		"Created new binary at path '" + targetBinary.string() + "'!",
		"COMMAND_CREATE",
		LogType::LOG_SUCCESS);
}

void Command_Get(
	Command_Get_Range range,
	const string& nameOrIndex,
	const path& targetBinary)
{

}

void Command_Replace(
	const string& nameOrIndex,
	const path& replaceWith,
	const path& targetBinary)
{

}

void Command_Remove(
	Command_Remove_Range range,
	const string& nameOrIndex,
	const path& targetBinary)
{

}

void Command_Reset(const path& targetBinary)
{

}

void Command_Compress(
	Command_Compress_Range range,
	const string& newFile,
	const string& name,
	const string& targetBinary)
{

}

void Command_Decompress(
	Command_Decompress_Range range,
	const string& nameOrIndex,
	const path& decompressTo,
	const path& targetBinary)
{

}