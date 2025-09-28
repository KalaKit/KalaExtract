//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <string>

#include "KalaHeaders/core_utils.hpp"

namespace KalaExtract
{
	using std::filesystem::path;
	using std::string;

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

	enum Command_Pack_Range
	{
		PACK_ALL,
		PACK_TARGET_WITH_NAME,
		PACK_TARGET_NO_NAME
	};

	enum Command_Unpack_Range
	{
		UNPACK_ALL,
		UNPACK_TARGET
	};

	class Data
	{
	public:
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

		static void Command_Pack(
			Command_Pack_Range range,
			bool willCompress,
			const string& newFile,
			const string& name,
			const string& targetBinary);

		static void Command_Unpack(
			Command_Unpack_Range range,
			bool willDecompress,
			const string& nameOrIndex,
			const path& decompressTo,
			const path& targetBinary);
	};
}