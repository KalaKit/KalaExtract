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

constexpr string_view GLOBAL_HEADER = "LOST_EMPIRE_BIN_";
constexpr string_view BUNDLE_START = "LOST_EMPIRE_STA_";
constexpr string_view BUNDLE_END = "LOST_EMPIRE_END_";

namespace KalaExtract
{
	bool Data::HasGlobalHeader(const path& targetPath)
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
}