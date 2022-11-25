#include <pugixml.hpp>

#include "ConfigurationSystem.h"
#include "EntityConfiguration.h"
#include "ControllerConfiguration.h"
#include "LevelConfiguration.h"

CConfigurationSystem::CConfigurationSystem(const std::filesystem::path& path)
	: m_pEntityConfiguration(std::make_unique<CEntityConfiguration>(path / "Entities.xml"))
	, m_pControllerConfiguration(std::make_unique<CControllerConfiguration>(path / "Controller.xml"))
	, m_pLevelConfiguration(std::make_unique<CLevelConfiguration>(path / "Levels.xml"))
{
	LoadPlayerConfiguration(path / "Player.xml");
}

CConfigurationSystem::~CConfigurationSystem() = default;

void CConfigurationSystem::LoadPlayerConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		std::cout << "Failed to load player configuration: " << res.description() << std::endl;
		return;
	}

	auto root = doc.child("Player");
	if (!root)
	{
		std::cout << "Invalid root element in player configuration" << std::endl;
		return;
	}

	m_playerConfiguration.fAccel = root.attribute("acceleration").as_float();
	m_playerConfiguration.fAngSpeed = root.attribute("angularSpeed").as_float();
	m_playerConfiguration.fProjSpeed = root.attribute("projectileSpeed").as_float();
	m_playerConfiguration.vShootHelper = root.attribute("shootHelper").as_vector();
	m_playerConfiguration.fShootCooldown = root.attribute("shootCooldown").as_float();
	m_playerConfiguration.fProjectileLifetime = root.attribute("projectileLifetime").as_float();
	m_playerConfiguration.dAmmoCount = root.attribute("ammoCount").as_int();
	m_playerConfiguration.fFuel = root.attribute("fuel").as_float();
	m_playerConfiguration.fConsumption = root.attribute("consumption").as_float();
}