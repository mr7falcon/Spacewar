#include "StdAfx.h"
#include "ResourceSystem.h"

CResourceSystem::CResourceSystem(const std::filesystem::path& path)
{
	LoadResource<sf::Image>(path / TexturesDirectory, m_textures,
		[](const std::string& path, sf::Image& texture) -> bool
		{
			return texture.loadFromFile(path);
		});

	LoadResource<sf::Font>(path / FontsDirectory, m_fonts,
		[](const std::string& path, sf::Font& font) -> bool
		{
			return font.loadFromFile(path);
		});

	LoadResource<sf::SoundBuffer>(path / SoundsDirectory, m_sounds,
		[](const std::string& path, sf::SoundBuffer& sound) -> bool
		{
			return sound.loadFromFile(path);
		});
}