#pragma once

#include <filesystem>
#include <map>

#include <SFML/Window/Keyboard.hpp>

class CControllerConfiguration
{
public:

	struct SConfiguration
	{
		sf::Keyboard::Key moveForward = sf::Keyboard::Unknown;
		sf::Keyboard::Key rotatePositive = sf::Keyboard::Unknown;
		sf::Keyboard::Key rotateNegative = sf::Keyboard::Unknown;
		sf::Keyboard::Key shoot = sf::Keyboard::Unknown;
	};

	CControllerConfiguration(const std::filesystem::path& path);
	CControllerConfiguration(const CControllerConfiguration&) = delete;

	const SConfiguration* GetConfiguration(const std::string& name) const;

private:

	std::map<std::string, SConfiguration> m_configurations;
};