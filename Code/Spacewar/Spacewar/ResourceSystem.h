#pragma once

#include <vector>
#include <map>
#include <functional>
#include <string>
#include <filesystem>
#include <iostream>

#include <SFML\Graphics\Image.hpp>

static constexpr int InvalidResourceId = -1;

class CResourceSystem
{
public:

	CResourceSystem(const std::filesystem::path& path);
	CResourceSystem(const CResourceSystem&) = delete;

	int GetTextureId(const std::string& path) const { return GetResourceId(m_textures, path); }
	const sf::Image* GetTexture(int id) const { return GetResource(m_textures, id); }

private:

	template <typename T>
	struct SResources
	{
		std::vector<T> m_resources;
		std::map<std::string, int> m_resourcesMap;
	};

private:

	template <typename T>
	void LoadResource(const std::filesystem::path& path, SResources<T>& resources, std::function<bool(const std::string&, T&)> load)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (!entry.is_directory())
			{

				T res;
				const std::string& resPath = entry.path().string();

				if (load(resPath, res))
				{
					resources.m_resources.push_back(std::move(res));
					resources.m_resourcesMap[resPath] = (int)(resources.m_resources.size() - 1);
				}
				else
				{
					std::cout << "Failed to load resource " << resPath << std::endl;
				}
			}
		}
	}
	
	template <typename T>
	inline int GetResourceId(const SResources<T>& resources, const std::string& path) const
	{
		std::filesystem::path tmp(path);
		auto fnd = resources.m_resourcesMap.find(tmp.make_preferred().string());
		return fnd != resources.m_resourcesMap.end() ? fnd->second : InvalidResourceId;
	}

	template <typename T>
	inline const T* GetResource(const SResources<T>& resources, int id) const
	{
		if (id >= 0 && id < resources.m_resources.size())
		{
			return &resources.m_resources[id];
		}
		return nullptr;
	}

private:

	SResources<sf::Image> m_textures;
};