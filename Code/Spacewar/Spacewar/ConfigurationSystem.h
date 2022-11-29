#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include <SFML/Graphics/Color.hpp>

class CEntityConfiguration;
class CControllerConfiguration;
class CLevelConfiguration;
class CPlayerConfiguration;

class CConfigurationSystem
{
public:

	CConfigurationSystem(const std::filesystem::path& path);
	CConfigurationSystem(const CConfigurationSystem&) = delete;
	~CConfigurationSystem();

	const CEntityConfiguration* GetEntityConfiguration() const { return m_pEntityConfiguration.get(); }
	const CControllerConfiguration* GetControllerConfiguration() const { return m_pControllerConfiguration.get(); }
	const CLevelConfiguration* GetLevelConfiguration() const { return m_pLevelConfiguration.get(); }
	const CPlayerConfiguration* GetPlayerConfiguration() const { return m_pPlayerConfiguration.get(); }

public:

	static sf::Color ParseColor(const std::string& color);		// make it inline in cpp file (extern)?

private:

	std::unique_ptr<CEntityConfiguration> m_pEntityConfiguration;
	std::unique_ptr<CControllerConfiguration> m_pControllerConfiguration;
	std::unique_ptr<CLevelConfiguration> m_pLevelConfiguration;
	std::unique_ptr<CPlayerConfiguration> m_pPlayerConfiguration;
};