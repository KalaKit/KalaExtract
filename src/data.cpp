//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/file_utils.hpp"
#include "KalaHeaders/string_utils.hpp"

#include "data.hpp"
#include "core.hpp"

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

//Global header magic at the start
constexpr string_view GLOBAL_HEADER = "LOST_EMPIRE_BIN";

//Local header magic at its start
constexpr string_view BUNDLE_START = "LOST_EMPIRE_STA";
//Local header magic at its end
constexpr string_view BUNDLE_END = "LOST_EMPIRE_END";

//Size of the global header without the magic
constexpr size_t SIZE_GLOBAL_HEADER = 276;

//All data stored at the top of a KalaExtract-created binary bundle collection
struct GlobalHeaderData
{
	//The magic word at the top of the global header, does not need to be filled manually
	char magic[16];

	//Total count of all bundles
	char size_bundles[256];

	//Size of all bundle raw data excluding their headers and end magic words
	u32 totalSize;

	GlobalHeaderData()
		: magic{},
		size_bundles{},
		totalSize(0)
	{
		size_bundles[0] = '0';
		size_bundles[1] = '\0';
	}
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

	//The magic word at the start of each data block, does not need to be filled manually
	char magic_start[16];

	//The magic word at the end of each data block, does not need to be filled manually
	char magic_end[16];

	BundleHeaderData()
		: name_bundle{},
		index_bundle{},
		size_compressed(0),
		size_decompressed(0),
		path_bundleOrigin{},
		pos_headerEnd(0),
		pos_endMagicStart(0),
		magic_start{},
		magic_end{} 
	{
		name_bundle[0] = '_';
		name_bundle[1] = '\0';

		index_bundle[0] = '0';
		index_bundle[1] = '\0';

		path_bundleOrigin[0] = '.';
		path_bundleOrigin[1] = '\0';

		magic_start[0] = '_';
		magic_start[1] = '\0';

		magic_end[0] = '_';
		magic_end[1] = '\0';
	}
};

static bool HasGlobalHeader(const path& target);

static bool AddGlobalHeader(const path& targetPath);

namespace KalaExtract
{
	void Data::Command_Create(const path& targetBinary)
	{
		ostringstream oss{};

		path base = Core::GetCurrentPath();
		path correctTarget = weakly_canonical(base / targetBinary);

		if (exists(correctTarget))
		{
			oss << "Failed to create target '" << targetBinary
				<< "' because the target already exists!";

			Log::Print(
				oss.str(),
				"COMMAND_CREATE",
				LogType::LOG_ERROR);

			return;
		}

		oss.str("");
		oss.clear();

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

		//add new global header
		AddGlobalHeader(correctTarget);

		Log::Print(
			"Created new target binary '" + targetBinary.string() + "'!",
			"COMMAND_CREATE",
			LogType::LOG_SUCCESS);
	}

	void Data::Command_Get(
		Command_Get_Range range,
		const string& nameOrIndex,
		const path& targetBinary)
	{
		ostringstream oss{};

		path base = Core::GetCurrentPath();
		path correctTarget = weakly_canonical(base / targetBinary);

		if (!exists(correctTarget))
		{
			oss << "Failed to get data from target '" << correctTarget
				<< "' because the target does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_GET",
				LogType::LOG_ERROR);

			return;
		}

		vector<BinaryRange> ranges{};
		string rangeResult = GetRangeByValue(
			correctTarget,
			GLOBAL_HEADER.data(),
			ranges);

		if (!rangeResult.empty())
		{
			oss << "Failed to get data from target '" << correctTarget
				<< "'! Reason: " << rangeResult;

			Log::Print(
				oss.str(),
				"COMMAND_GET",
				LogType::LOG_ERROR);

			return;
		}

		if (ranges.empty())
		{
			oss << "Failed to get data from target '" << correctTarget
				<< "' because it does not have a global header.";

			Log::Print(
				oss.str(),
				"COMMAND_GET",
				LogType::LOG_WARNING);

			return;
		}

		//
		// FOUND GLOBAL HEADER, GETTING DATA
		//

		//skip the magic, start at struct body
		size_t headerStart = ranges[0].start;

		vector<u8> linesData{};
		string linesResult = ReadBinaryLinesFromFile(
			correctTarget,
			linesData,
			headerStart,
			headerStart + SIZE_GLOBAL_HEADER);

		if (!linesResult.empty())
		{
			oss << "Failed to get data from target '" << correctTarget
				<< "'! Reason: " << linesResult;

			Log::Print(
				oss.str(),
				"COMMAND_GET",
				LogType::LOG_ERROR);

			return;
		}

		if (linesData.size() != sizeof(GlobalHeaderData))
		{
			oss << "Failed to get data from target '" << correctTarget
				<< "'! Gathered line data size '" << linesData.size()
				<< "' does not match required size '" << sizeof(GlobalHeaderData) << "'!";

			Log::Print(
				oss.str(),
				"COMMAND_GET",
				LogType::LOG_ERROR);

			return;
		}

		GlobalHeaderData header{};
		memcpy(&header, linesData.data(), sizeof(GlobalHeaderData));

		string bundleCount(
			header.size_bundles,
			header.size_bundles + sizeof(header.size_bundles));
		bundleCount = string(bundleCount.c_str());

		oss << "Global header info for '" << correctTarget << "':\n"
			<< "  Magic: " << string(header.magic, sizeof(header.magic)) << "\n"
			<< "  Bundle count: " << bundleCount << "\n"
			<< "  Total size: " << header.totalSize << " bytes";

		Log::Print(
			oss.str(),
			"COMMAND_GET",
			LogType::LOG_SUCCESS);
	}

	void Data::Command_Replace(
		const string& nameOrIndex,
		const path& replaceWith,
		const path& targetBinary)
	{
		ostringstream oss{};

		path base = Core::GetCurrentPath();
		path correctTarget = weakly_canonical(base / targetBinary);

		if (!exists(correctTarget))
		{
			oss << "Failed to replace a bundle from target '" << correctTarget
				<< "' because the target does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_REPLACE",
				LogType::LOG_ERROR);

			return;
		}

		oss.str("");
		oss.clear();

		oss << "Replaced a bundle in target '" << correctTarget << "' !";

		Log::Print(
			oss.str(),
			"COMMAND_REPLACE",
			LogType::LOG_SUCCESS);
	}

	void Data::Command_Remove(
		Command_Remove_Range range,
		const string& nameOrIndex,
		const path& targetBinary)
	{
		ostringstream oss{};

		path base = Core::GetCurrentPath();
		path correctTarget = weakly_canonical(base / targetBinary);

		if (!exists(correctTarget))
		{
			oss << "Failed to remove a bundle from target '" << correctTarget
				<< "' because the target does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_REMOVE",
				LogType::LOG_ERROR);

			return;
		}

		oss.str("");
		oss.clear();

		oss << "Removed a bundle from target '" << correctTarget << "' !";

		Log::Print(
			oss.str(),
			"COMMAND_REMOVE",
			LogType::LOG_SUCCESS);
	}

	void Data::Command_Reset(const path& targetBinary)
	{
		ostringstream oss{};

		path base = Core::GetCurrentPath();
		path correctTarget = weakly_canonical(base / targetBinary);

		if (!exists(correctTarget))
		{
			oss << "Failed to reset target '" << correctTarget
				<< "' because the target does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_RESET",
				LogType::LOG_ERROR);

			return;
		}
		else correctTarget = weakly_canonical(correctTarget);

		oss.str("");
		oss.clear();

		oss << "Reset target '" << correctTarget << "' !";

		Log::Print(
			oss.str(),
			"COMMAND_RESET",
			LogType::LOG_SUCCESS);
	}

	void Data::Command_Pack(
		Command_Pack_Range range,
		bool willCompress,
		const path& newFile,
		const string& name,
		const path& targetBinary)
	{
		ostringstream oss{};

		path base = Core::GetCurrentPath();
		path correctTarget = weakly_canonical(base / targetBinary);

		if (!exists(correctTarget))
		{
			oss << "Failed to pack to target '" << correctTarget
				<< "' because the target does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_PACK",
				LogType::LOG_ERROR);

			return;
		}
		else correctTarget = weakly_canonical(correctTarget);

		if (range != Command_Pack_Range::PACK_ALL
			&& !exists(newFile))
		{
			oss << "Failed to pack new file '" << newFile << "' to target '" << correctTarget
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
			oss << "Failed to pack new file '" << newFile << "' to target '" << correctTarget
				<< "' because the assigned name is empty!";

			Log::Print(
				oss.str(),
				"COMMAND_PACK",
				LogType::LOG_ERROR);

			return;
		}

		oss.str("");
		oss.clear();

		string targetName = name;
		if (targetName.empty())
		{
			targetName = path(newFile).stem().string();
		}
		oss << "Starting to pack file or folder '" << newFile
			<< "' with name '" << targetName << "' to target '" << correctTarget.stem() << "'.";

		Log::Print(
			oss.str(),
			"COMMAND_PACK",
			LogType::LOG_INFO);

		oss.str("");
		oss.clear();

		//add new global header if one doesnt already exist
		AddGlobalHeader(correctTarget);

		oss << "Packed target '" << correctTarget << "' !";

		Log::Print(
			oss.str(),
			"COMMAND_PACK",
			LogType::LOG_SUCCESS);
	}

	void Data::Command_Unpack(
		Command_Unpack_Range range,
		bool willDecompress,
		const string& nameOrIndex,
		const path& decompressTo,
		const path& targetBinary)
	{
		ostringstream oss{};

		path base = Core::GetCurrentPath();
		path correctTarget = weakly_canonical(base / targetBinary);

		if (!exists(correctTarget))
		{
			oss << "Failed to unpack from target '" << correctTarget
				<< "' because the target does not exist!";

			Log::Print(
				oss.str(),
				"COMMAND_UNPACK",
				LogType::LOG_ERROR);

			return;
		}
		else correctTarget = weakly_canonical(correctTarget);

		oss.str("");
		oss.clear();

		oss << "Unpacked to target '" << correctTarget << "' !";

		Log::Print(
			oss.str(),
			"COMMAND_UNPACK",
			LogType::LOG_SUCCESS);
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

	constexpr size_t magicSize = 16;
	size_t headerSize{};
	size_t totalHeaderSize{};

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

		memcpy(header.magic, "LOST_EMPIRE_BIN", 16);

		headerSize = sizeof(header) - magicSize;  //260
		totalHeaderSize = sizeof(header);         //276

		out.write(reinterpret_cast<const char*>(&header), totalHeaderSize);

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

	oss << "Added global header to target '" << targetBinary << "'."
		<< " Size: " << totalHeaderSize 
		<< " (magic: " << magicSize 
		<< ", body: " << headerSize << ")";

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
		bool ignoreError = ContainsString(
			result, 
			"because target file is empty", 
			false);

		ostringstream oss{};

		if (ignoreError)
		{
			oss << "Ignoring false positive error 'target file is empty' from 'GetRangeByValue' function.";

			Log::Print(
				oss.str(),
				"DATA",
				LogType::LOG_DEBUG);
		}
		else
		{
			oss << "Failed to check if target path '" << targetBinary
				<< "' has a global header or not! Reason: " << result;

			Log::Print(
				oss.str(),
				"DATA",
				LogType::LOG_ERROR);

			return false;
		}
	}

	return !outData.empty();
}