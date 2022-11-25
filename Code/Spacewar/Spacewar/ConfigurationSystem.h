#pragma once

#include <filesystem>
#include <memory>

class CEntityConfiguration;
class CControllerConfiguration;
class CLevelConfiguration;

class CConfigurationSystem
{
public:

	struct SPlayerConfiguration			// Move it in CPlayerConfiguration along with the other configurations to choose
	{
		float fAccel = 5.f;
		float fAngSpeed = 5.f;
		float fProjSpeed = 10.f;
		sf::Vector2f vShootHelper;
		float fShootCooldown = 1.f;
		float fProjectileLifetime = 4.f;
		int dAmmoCount = -1;
		float fFuel = -1.f;
		float fConsumption = 0.f;
	};

	CConfigurationSystem(const std::filesystem::path& path);
	CConfigurationSystem(const CConfigurationSystem&) = delete;
	~CConfigurationSystem();

	const CEntityConfiguration* GetEntityConfiguration() const { return m_pEntityConfiguration.get(); }
	const CControllerConfiguration* GetControllerConfiguration() const { return m_pControllerConfiguration.get(); }
	const CLevelConfiguration* GetLevelConfiguration() const { return m_pLevelConfiguration.get(); }
	const SPlayerConfiguration& GetPlayerConfiguration() const { return m_playerConfiguration; }

private:

	void LoadPlayerConfiguration(const std::filesystem::path& path);

private:

	std::unique_ptr<CEntityConfiguration> m_pEntityConfiguration;
	std::unique_ptr<CControllerConfiguration> m_pControllerConfiguration;
	std::unique_ptr<CLevelConfiguration> m_pLevelConfiguration;
	SPlayerConfiguration m_playerConfiguration;
};