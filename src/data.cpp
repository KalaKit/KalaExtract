//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/file_utils.hpp"

#include "data.hpp"

using namespace KalaHeaders;

using std::string_view;
using std::vector;
using std::ostringstream;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::ios;
using std::memcmp;
using std::begin;
using std::end;

constexpr string_view GLOBAL_HEADER = "LOST_EMPIRE_BIN";
constexpr string_view BUNDLE_START = "LOST_EMPIRE_STA";
constexpr string_view BUNDLE_END = "LOST_EMPIRE_END";

//All data stored at the top of a KalaExtract-created binary bundle collection
struct GlobalHeaderData
{
	//Total count of all bundles
	char size_bundles[256];

	//Size of all bundle raw data excluding their headers and end magic words
	u32 totalSize;
};

//All data stored at the top of each KalaExtract-created binary bundle
struct BundleHeaderData
{
	//Name of this bundle
	char name_bundle[256];

	//Which bundle is this
	char index_bundle[256];

	//Size of compressed bundle
	u32 size_compressed;
	//Size of decompressed bundle
	u32 size_decompressed;

	//Path where bundle originally came from
	char path_bundleOrigin[256];

	//Bytes from bundle header end to bundle end magic start
	u32 pos_headerEnd;
	//Bytes from bundle end magic start to bundle header end
	u32 pos_endMagicStart;
};

static bool HasGlobalHeader(const path& target);

static bool AddGlobalHeader(const path& targetPath);

namespace KalaExtract
{
	void Data::Command_Create(const path& targetBinary)
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

	void Data::Command_Get(
		Command_Get_Range range,
		const string& nameOrIndex,
		const path& targetBinary)
	{

	}

	void Data::Command_Replace(
		const string& nameOrIndex,
		const path& replaceWith,
		const path& targetBinary)
	{

	}

	void Data::Command_Remove(
		Command_Remove_Range range,
		const string& nameOrIndex,
		const path& targetBinary)
	{

	}

	void Data::Command_Reset(const path& targetBinary)
	{

	}

	void Data::Command_Pack(
		Command_Pack_Range range,
		bool willCompress,
		const path& newFile,
		const string& name,
		const path& targetBinary)
	{
		ostringstream oss{};

		if (range != Command_Pack_Range::PACK_ALL
			&& !exists(newFile))
		{
			oss << "Failed to pack new file '" << newFile << "' to target '" << targetBinary
				<< "' because the new file does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_PACK",
				LogType::LOG_ERROR);

			return;
		}
		if (range == Command_Pack_Range::PACK_TARGET_WITH_NAME
			&& name.empty())
		{
			oss << "Failed to pack new file '" << newFile << "' to target '" << targetBinary
				<< "' because the assigned name is empty!";

			Log::Print(
				oss.str(),
				"COMMAND_PACK",
				LogType::LOG_ERROR);

			return;
		}
		if (!exists(targetBinary))
		{
			oss << "Failed to pack new file '" << newFile << "' to target '" << targetBinary
				<< "' because the target does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_PACK",
				LogType::LOG_ERROR);

			return;
		}

		string targetName = name;
		if (targetName.empty())
		{
			targetName = path(newFile).stem().string();
		}
		oss << "Starting to pack file or folder '" << newFile
			<< "' with name '" << targetName << "' to target '" << targetBinary.stem() << "'.";

		Log::Print(
			oss.str(),
			"COMMAND_PACK",
			LogType::LOG_INFO);

		//add new global header if one doesnt already exist
		AddGlobalHeader(targetBinary);
	}

	void Data::Command_Unpack(
		Command_Unpack_Range range,
		bool willDecompress,
		const string& nameOrIndex,
		const path& decompressTo,
		const path& targetBinary)
	{

	}
}

bool AddGlobalHeader(const path& targetBinary)
{
	//skip if global header already exists
	if (HasGlobalHeader(targetBinary)) return false;

	ostringstream oss{};

	oss << "Did not find global header from target '" << targetBinary
		<< "', adding new global header.";

	Log::Print(
		oss.str(),
		"GLOBAL_HEADER",
		LogType::LOG_INFO);

	oss.str("");
	oss.clear();

	try
	{
		ofstream out(
			targetBinary,
			ios::binary
			| ios::out
			| ios::app);

		if (out.fail()
			&& errno != 0)
		{
			int err = errno;

			oss << "Failed to place global header to target '"
				<< targetBinary << "' because it couldn't be opened! Reason: " << strerror(err);

			Log::Print(
				oss.str(),
				"GLOBAL_HEADER",
				LogType::LOG_ERROR);

			return false;
		}

		GlobalHeaderData header{};

		const char magic[16] = "LOST_EMPIRE_BIN";
		memset(header.size_bundles, 0, sizeof(header.size_bundles));
		header.totalSize = 0;

		out.write(magic, sizeof(magic));
		out.write(reinterpret_cast<const char*>(&header), sizeof(header));

		if (out.fail()
			&& errno != 0)
		{
			int err = errno;

			oss << "Failed to place global header to target '"
				<< targetBinary << "' because a write error occured! Reason: " << strerror(err);

			Log::Print(
				oss.str(),
				"GLOBAL_HEADER",
				LogType::LOG_ERROR);

			return false;
		}
	}
	catch (exception& e)
	{
		oss << "Failed to add global header to target '" << targetBinary << "'! Reason: " << e.what();

		return false;
	}

	oss << "Added global header to target '" << targetBinary << "'.";

	Log::Print(
		oss.str(),
		"GLOBAL_HEADER",
		LogType::LOG_SUCCESS);

	return true;
}

bool HasGlobalHeader(const path& targetBinary)
{
	string headerWithNull(GLOBAL_HEADER);
	headerWithNull.push_back('\0');

	vector<BinaryRange> outData{};

	string result = GetRangeByValue(
		targetBinary,
		headerWithNull,
		outData);

	if (!result.empty())
	{
		ostringstream oss{};

		oss << "Failed to check if target path '" << targetBinary
			<< "' has a global header or not! Reason: " << result;

		Log::Print(
			oss.str(),
			"DATA",
			LogType::LOG_ERROR);

		return false;
	}

	return !outData.empty();
}