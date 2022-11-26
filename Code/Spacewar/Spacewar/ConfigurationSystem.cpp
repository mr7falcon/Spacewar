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