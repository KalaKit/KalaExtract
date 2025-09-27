//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <filesystem>

#include "core/command.hpp"

using std::string;
using std::vector;
using std::filesystem::path;

enum Command_Get_Range
{
	GET_ALL,
	GET_TARGET
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

static void Command_Create(const path& targetBinary);

static void Command_Get(
	Command_Get_Range range,
	const string& nameOrIndex,
	const path& targetBinary);

static void Command_Replace(
	const string& nameOrIndex,
	const path& replaceWith,
	const path& targetBinary);

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
	void Command::RunCommand(const vector<string>& params)
	{

	}
}