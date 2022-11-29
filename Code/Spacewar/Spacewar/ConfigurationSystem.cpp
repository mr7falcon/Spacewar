#include <pugixml.hpp>

#include "ConfigurationSystem.h"
#include "EntityConfiguration.h"
#include "ControllerConfiguration.h"
#include "LevelConfiguration.h"
#include "PlayerConfiguration.h"

CConfigurationSystem::CConfigurationSystem(const std::filesystem::path& path)
	: m_pEntityConfiguration(std::make_unique<CEntityConfiguration>(path / "Entities.xml"))
	, m_pControllerConfiguration(std::make_unique<CControllerConfiguration>(path / "Controller.xml"))
	, m_pLevelConfiguration(std::make_unique<CLevelConfiguration>(path / "Levels.xml"))
	, m_pPlayerConfiguration(std::make_unique<CPlayerConfiguration>(path / "Players.xml"))
{}

CConfigurationSystem::~CConfigurationSystem() = default;

sf::Color CConfigurationSystem::ParseColor(const std::string& color)
{
	if (color == "White")
		return sf::Color::White;
	if (color == "Black")
		return sf::Color::Black;
	if (color == "Red")
		return sf::Color::Red;
	if (color == "Blue")
		return sf::Color::Blue;
	if (color == "Green")
		return sf::Color::Green;
	if (color == "Yellow")
		return sf::Color::Yellow;
	return sf::Color::Black;
}