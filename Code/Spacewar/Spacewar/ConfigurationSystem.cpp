#include "ConfigurationSystem.h"
#include "EntityConfiguration.h"

CConfigurationSystem::CConfigurationSystem(const std::filesystem::path& path)
	: m_pEntityConfiguration(std::make_unique<CEntityConfiguration>(path / "Entities.xml"))
{}

CConfigurationSystem::~CConfigurationSystem() = default;