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

	class Data
	{
	public:
		//Adds a new global header to the target binary
		static bool AddGlobalHeader(const path& targetPath);

		//Adds a new bundle to the target binary
		static bool AddBundle(
			const BundleHeaderData& newBundleData,
			const string& bundleNameOrIndex,
			const string& targetBinary);

		//Removes the existing bundle from the target binary
		static bool RemoveBundle(
			const string& bundleNameOrIndex,
			const string& targetBinary);

		//Extracts the existing bundle data from the target binary
		static bool GetBundleData(
			const BundleHeaderData& newBundleData,
			const string& bundleNameOrIndex,
			const string& targetBinary);

		//Removes the global header and all bundles from the target binary
		static bool ResetBinary(const path& target);
	};
}