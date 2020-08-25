#pragma once

#include "olcPixelGameEngine.h"
#include <string>
#include <fstream>
#include <map>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem::v1;



class SpriteManager
{
	using SpriteMap = std::map<std::string, olc::Decal*>;
	using iterator = fs::directory_iterator;
	using recursive_iterator = fs::recursive_directory_iterator;
	using entry = fs::directory_entry;

private:
	SpriteMap m_mapSprites;
	std::string m_sCurrentFolder;
	
public:
	SpriteManager();
	
private:
	static bool StringEndsWith(const std::string& str, const std::string& ending);
	static std::string CurrentFolderName();

public:
	bool contains(const std::string& sFilename);

	bool loadPngFile(const std::string& sFilename);
	bool loadPgeFile(const std::string& sFilename);
	bool loadDirectionary(std::string& sDirectionaryName);
	bool loadDirectionaryRecursive(std::string& sDirectionaryName);

	olc::Decal* query(const std::string& sFilename);

	bool freeFile(const std::string& sFilename);
	bool freeAll();
};

