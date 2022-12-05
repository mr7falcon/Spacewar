#pragma once

#include "Controllers/Controller.h"

#include <filesystem>
#include <map>
#include <SFML/Window/Keyboard.hpp>

/**
 * @class CControllerConfiguration
 * Class, loading and storing keyboard controllers' configurations.
 * Also used to provide an unified interface for the any type controller creation.
 */
class CControllerConfiguration
{
public:

	struct SConfiguration
	{
		sf::Keyboard::Key moveForward = sf::Keyboard::Unknown;
		sf::Keyboard::Key moveBack = sf::Keyboard::Unknown;
		sf::Keyboard::Key rotatePositive = sf::Keyboard::Unknown;
		sf::Keyboard::Key rotateNegative = sf::Keyboard::Unknown;
		sf::Keyboard::Key shoot = sf::Keyboard::Unknown;
		sf::Keyboard::Key escape = sf::Keyboard::Unknown;
	};

	CControllerConfiguration(const std::filesystem::path& path);
	CControllerConfiguration(const CControllerConfiguration&) = delete;

	const SConfiguration* GetConfiguration(const std::string& name) const;

	std::shared_ptr<CController> CreateController(const std::string& config) const;
	std::shared_ptr<CController> CreateDefaultController() const;

private:

	std::map<std::string, SConfiguration> m_configurations;
};