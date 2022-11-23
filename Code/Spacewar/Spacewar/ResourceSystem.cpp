#include <filesystem>

#include "ResourceSystem.h"

CResourceSystem::CResourceSystem(const std::filesystem::path& path)
{
	LoadResource<sf::Image>(path / "Textures", m_textures,
		[](const std::string& path, sf::Image& texture) -> bool
		{
			return texture.loadFromFile(path);
		});
}