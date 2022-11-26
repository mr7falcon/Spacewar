#include <pugixml.hpp>
#include <iostream>

#include "PlayerConfiguration.h"

CPlayerConfiguration::CPlayerConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		std::cout << "Failed to load players configuration: " << res.description() << std::endl;
		return;
	}

	auto root = doc.child("Players");
	if (!root)
	{
		std::cout << "Invalid root element in players configuration" << std::endl;
		return;
	}

	for (auto iter = root.begin(); iter != root.end(); ++iter)
	{
		std::string name = iter->attribute("name").value();
		if (!name.empty())
		{
			auto fnd = m_configurations.find(name);
			if (fnd != m_configurations.end())
			{
				std::cout << "Player configuration with name " << name << " already exists" << std::endl;
				continue;
			}

			SPlayerConfiguration config;

			config.entityName = iter->attribute("entity").value();
			config.fAccel = iter->attribute("acceleration").as_float();
			config.fAngSpeed = iter->attribute("angularSpeed").as_float();
			config.fProjSpeed = iter->attribute("projectileSpeed").as_float();
			config.vShootHelper = iter->attribute("shootHelper").as_vector();
			config.fShootCooldown = iter->attribute("shootCooldown").as_float();
			config.dNumShotsInBurst = iter->attribute("shotsInBurst").as_int();
			config.fBurstCooldown = iter->attribute("burstCooldown").as_float();
			config.fProjectileLifetime = iter->attribute("projectileLifetime").as_float();
			config.dAmmoCount = iter->attribute("ammoCount").as_int();
			config.fFuel = iter->attribute("fuel").as_float();
			config.fConsumption = iter->attribute("consumption").as_float();

			m_configurations[std::move(name)] = std::move(config);
		}
	}
}

const CPlayerConfiguration::SPlayerConfiguration* CPlayerConfiguration::GetConfiguration(const std::string& name) const
{
	auto fnd = m_configurations.find(name);
	return fnd != m_configurations.end() ? &fnd->second : nullptr;
}