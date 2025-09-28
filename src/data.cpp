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
	char size_bundles[255];

	//Size of all bundle raw data excluding their headers and end magic words
	u32 totalSize;
};

//All data stored at the top of each KalaExtract-created binary bundle
struct BundleHeaderData
{
	//Name of this bundle
	char name_bundle[255];

	//Which bundle is this
	char index_bundle[255];

	//Size of compressed bundle
	u32 size_compressed;
	//Size of decompressed bundle
	u32 size_decompressed;

	//Path where bundle originally came from
	char path_bundleOrigin[255];

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
		const string& newFile,
		const string& name,
		const string& targetBinary)
	{
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

bool AddGlobalHeader(const path& targetPath)
{
	//skip if global header already exists
	if (HasGlobalHeader(targetPath)) return false;

	return true;
}

bool HasGlobalHeader(const path& targetPath)
{
	string headerWithNull(GLOBAL_HEADER);
	headerWithNull.push_back('\0');

	vector<BinaryRange> outData{};

	string result = GetRangeByValue(
		targetPath,
		headerWithNull,
		outData);

	if (!result.empty())
	{
		ostringstream oss{};

		oss << "Failed to check if target path '" << targetPath
			<< "' has a global header or not! Reason: " << result;

		Log::Print(
			oss.str(),
			"DATA",
			LogType::LOG_ERROR);

		return false;
	}

	return !outData.empty();
}