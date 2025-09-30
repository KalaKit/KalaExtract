//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>

namespace KalaExtract
{
	using std::filesystem::path;
	using std::filesystem::current_path;

	class Core
	{
	public:
		static void Run(int argc, char* argv[]);

		static inline void SetCurrentPath(const path& newPath)
		{
			currentPath = newPath;
		}
		static inline const path& GetCurrentPath()
		{
			if (currentPath.empty()) currentPath = current_path();
			return currentPath;
		}
	private:
		static inline path currentPath;
	};
}