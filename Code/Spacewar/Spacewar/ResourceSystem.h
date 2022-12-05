#pragma once

#include <vector>
#include <map>
#include <functional>
#include <string>
#include <filesystem>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

static constexpr int InvalidResourceId = -1;

/**
 * @class CResourceSystem
 * Class to load, store, and provide access for all the game resources:
 * textures, fonts and sounds.
 */
class CResourceSystem
{
public:

	CResourceSystem(const std::filesystem::path& path);
	CResourceSystem(const CResourceSystem&) = delete;

	int GetTextureId(const std::string& path) const { return GetResourceId(m_textures, path); }
	const sf::Image* GetTexture(int id) const { return GetResource(m_textures, id); }
	std::vector<int> GetTexturesInDirectory(const std::string& subDir) const { return GetResourcesInDirectory(m_textures, std::filesystem::path(TexturesDirectory) / subDir); }

	int GetFontId(const std::string& path) const { return GetResourceId(m_fonts, path); }
	const sf::Font* GetFont(int id) const { return GetResource(m_fonts, id); }

	int GetSoundId(const std::string& path) const { return GetResourceId(m_sounds, path); }
	const sf::SoundBuffer* GetSound(int id) const { return GetResource(m_sounds, id); }

private:

	static constexpr const char* TexturesDirectory = "Textures";
	static constexpr const char* FontsDirectory = "Fonts";
	static constexpr const char* SoundsDirectory = "Sounds";

	/**
	 * @struct SResources
	 * Template struct, containing the resources vector, and the path-index mapping.
	 * 
	 * @template param T - type of the resource to store.
	 */
	template <typename T>
	struct SResources
	{
		std::vector<T> m_resources;
		std::map<std::string, int> m_resourcesMap;
	};

private:

	/**
	 * @function LoadResource
	 * Load all resources in the specified folder.
	 * 
	 * @template param T - resource type.
	 * @param path - base folder where load resources from
	 * @param resources - resources container with respect to the specified type
	 * @param load - loading function
	 */
	template <typename T>
	void LoadResource(const std::filesystem::path& path, SResources<T>& resources, std::function<bool(const std::string&, T&)> load)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (!entry.is_directory())
			{
				T res;
				std::string resPath = entry.path().string();

				if (load(resPath, res))
				{
					resources.m_resources.push_back(std::move(res));
					resources.m_resourcesMap[resPath] = (int)(resources.m_resources.size() - 1);
				}
				else
				{
					Log("Failed to load resource ", resPath);
				}
			}
		}
	}
	
	// Get the resource id from the specified container corresponding to the specified path
	template <typename T>
	inline int GetResourceId(const SResources<T>& resources, std::filesystem::path path) const
	{
		auto fnd = resources.m_resourcesMap.find(path.make_preferred().string());
		return fnd != resources.m_resourcesMap.end() ? fnd->second : InvalidResourceId;
	}

	// Get the resource from the specified container corresponding to the specified identifier
	template <typename T>
	inline const T* GetResource(const SResources<T>& resources, int id) const
	{
		if (id >= 0 && id < resources.m_resources.size())
		{
			return &resources.m_resources[id];
		}
		return nullptr;
	}

	// Get all the resources from the specified container which are contained in the specified directory
	template <typename T>
	std::vector<int> GetResourcesInDirectory(const SResources<T>& resources, std::filesystem::path dir) const
	{
		std::vector<int> res;
		for (const auto& [path, id] : resources.m_resourcesMap)
		{
			if (path.find(dir.make_preferred().string()) != std::string::npos)
			{
				res.push_back(id);
			}
		}
		return res;
	}

private:

	SResources<sf::Image> m_textures;
	SResources<sf::Font> m_fonts;
	SResources<sf::SoundBuffer> m_sounds;
};