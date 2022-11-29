#pragma once

#include <filesystem>
#include <map>

#include <SFML/Window/Keyboard.hpp>

#include "IController.h"

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

	std::shared_ptr<IController> CreateController(const std::string& config) const;
	std::shared_ptr<IController> CreateDefaultController() const;

private:

	std::map<std::string, SConfiguration> m_configurations;
};