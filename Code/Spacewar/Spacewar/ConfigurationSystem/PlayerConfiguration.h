#pragma once

#include <filesystem>
#include <string>
#include <map>

#include <SFML/System/Vector2.hpp>

/**
 * @class CPlayerConfiguration
 * Class is used for loading and storing players' configurations.
 */
class CPlayerConfiguration
{
public:

	struct SPlayerConfiguration
	{
		std::string entityName;
		std::string projectileEntityName;
		std::string feedbackSchema;
		float fAccel = 5.f;
		float fAngSpeed = 5.f;
		float fProjSpeed = 10.f;
		sf::Vector2f vShootHelper;
		float fShootCooldown = 1.f;
		int numShotsInBurst = 1;
		float fBurstCooldown = 0.4f;
		float fProjectileLifetime = 4.f;
		int ammoCount = -1;
		float fFuel = -1.f;
		float fConsumption = 0.f;
	};

	CPlayerConfiguration(const std::filesystem::path& path);
	CPlayerConfiguration(const CPlayerConfiguration&) = delete;

	const SPlayerConfiguration* GetConfiguration(const std::string& name) const;

	const std::string& GetDefaultConfiguration() const;
	const std::string& GetNextConfiguration(const std::string& current) const;
	const std::string& GetPreviousConfiguration(const std::string& current) const;

private:

	std::map<std::string, SPlayerConfiguration> m_configurations;
};