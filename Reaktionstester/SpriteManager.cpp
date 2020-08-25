#include "SpriteManager.h"

SpriteManager::SpriteManager()
{
    m_mapSprites = SpriteMap();
    m_sCurrentFolder = CurrentFolderName();
}

bool SpriteManager::StringEndsWith(const std::string& str, const std::string& ending)
{
    uint64_t diff = str.size() - ending.size();
    for (uint64_t i = diff; i < str.size(); i++)
    {
        if (str.at(i) != ending.at(i - diff)) return false;
    }
    return true;
}

std::string SpriteManager::CurrentFolderName()
{
    std::string sPath = fs::current_path().string();

    uint64_t nLastSlash = 0;
    for (uint32_t i = 0; i < sPath.size(); i++)
    {
        if (sPath.at(i) == '/' || sPath.at(i) == '\\')
            nLastSlash = i;
    }

    return sPath.substr(nLastSlash + 1);
}

bool SpriteManager::contains(const std::string& sFilename)
{
    return m_mapSprites.find(sFilename) != m_mapSprites.end();
}

bool SpriteManager::loadPngFile(const std::string& sFilename)
{
    if (contains(sFilename)) return false;

    olc::Decal* decNew = new olc::Decal(new olc::Sprite(sFilename));
    if (decNew != nullptr)
    {
        m_mapSprites[sFilename] = decNew;
        return true;
    }
    
    return false;
}

bool SpriteManager::loadPgeFile(const std::string& sFilename)
{
    if (contains(sFilename)) return false;

    olc::Sprite* spr = new olc::Sprite();
    spr->LoadFromFile(sFilename);
    olc::Decal* decNew = new olc::Decal(spr);

    if (decNew != nullptr)
    {
        m_mapSprites[sFilename] = decNew;
        return true;
    }

    return false;
}

bool SpriteManager::loadDirectionary(std::string& sDirectionaryName)
{
    uint32_t nOffset = 0;
    if (sDirectionaryName == "")
    {
        sDirectionaryName = "../" + m_sCurrentFolder;
        nOffset = sDirectionaryName.size() + 1;
    }

    for (entry entry : iterator(sDirectionaryName))
    {
        if (StringEndsWith(entry.path().string(), ".png"))
            loadPngFile(entry.path().string().substr(nOffset));
        else if (StringEndsWith(entry.path().string(), ".pgepng"))
            loadPgeFile(entry.path().string().substr(nOffset));
    }

    return true;
}

bool SpriteManager::loadDirectionaryRecursive(std::string& sDirectionaryName)
{
    uint32_t nOffset = 0;
    if (sDirectionaryName == "")
    {
        sDirectionaryName = "../" + m_sCurrentFolder;
        nOffset = sDirectionaryName.size() + 1;
    }

    for (entry entry : recursive_iterator(sDirectionaryName))
    {
        if (StringEndsWith(entry.path().string(), ".png"))
            loadPngFile(entry.path().string().substr(nOffset));
        else if (StringEndsWith(entry.path().string(), ".pgepng"))
            loadPgeFile(entry.path().string().substr(nOffset));
    }

    return true;
}

olc::Decal* SpriteManager::query(const std::string& sFilename)
{
    if (contains(sFilename))
        return m_mapSprites.at(sFilename);

    return nullptr;
}

bool SpriteManager::freeFile(const std::string& sFilename)
{
    if (contains(sFilename))
    {
        delete query(sFilename);
        m_mapSprites.erase(sFilename);
    }
    
    return false;
}

bool SpriteManager::freeAll()
{
    for (auto& entry : m_mapSprites)
        delete entry.second;
    m_mapSprites.clear();

    return true;
}
