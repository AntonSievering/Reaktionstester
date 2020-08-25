// <===============================>
// | @Author: Anton Sievering      |
// | @Date: 25. August 2020        |
// | @Version: 1.0f                |
// <===============================>


#pragma once

// some includes
#include "olcPixelGameEngine.h"
#include <string>
#include <fstream>
#include <map>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

// to shorten things
namespace fs = std::experimental::filesystem::v1;


// <===========================================================================>
// | Class SpriteManager - a manager that loads and holds png and pgepng files |
// | Usage: using the methods that begin with "load", you can load sprites.    |
// |        with the "query" method, you can get the pointer to the olc::Decal |
// |        with the "free" methods, you can delete olc::Decals                |
// |        with the "contains" method, you can check, whether a file has been |
// |        loaded yet                                                         |
// <===========================================================================>
class SpriteManager
{
	// custom typedefs
	using SpriteMap          = std::map<std::string, olc::Decal*>;
	using iterator           = fs::directory_iterator;
	using recursive_iterator = fs::recursive_directory_iterator;
	using entry              = fs::directory_entry;

private:
	// the map that holds the filename (as key) and the olc::Decal* (as value)
	SpriteMap m_mapSprites;
	// the current folder name is in use in the loading methods
	std::string m_sCurrentFolder;
	
public:
	// Standard Constructer
	SpriteManager();
	
private:
	// some private utility methods
	static bool StringEndsWith(const std::string& str, const std::string& ending);
	static std::string CurrentFolderName();

public:
	// checks whether the manager already loaded a file
	bool contains(const std::string& sFilename);

	// can load png and pgepng files
	// if the file was not found or already loaded, the return false
	bool loadPngFile(const std::string& sFilename);
	bool loadPgeFile(const std::string& sFilename);
	bool loadDirectionary(std::string& sDirectionaryName);
	bool loadDirectionaryRecursive(std::string& sDirectionaryName);

	// return a pointer to an olc::Decal
	// return nullptr if the file was not loaded yet
	olc::Decal* query(const std::string& sFilename);

	// can delete assets
	// return true
	bool freeFile(const std::string& sFilename);
	bool freeAll();
};

