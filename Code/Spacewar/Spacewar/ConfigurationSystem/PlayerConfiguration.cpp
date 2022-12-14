#include "StdAfx.h"
#include "Game.h"
#include "PlayerConfiguration.h"

CPlayerConfiguration::CPlayerConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		Log("Failed to load players configuration: ", res.description());
		return;
	}

	auto root = doc.child("Players");
	if (!root)
	{
		Log("Invalid root element in players configuration");
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
				Log("Player configuration with name ", name, " already exists");
				continue;
			}

			SPlayerConfiguration config;

			config.entityName = iter->attribute("entity").value();
			config.projectileEntityName = iter->attribute("projectileEntity").value();
			config.feedbackSchema = iter->attribute("feedbackSchema").value();
			config.fAccel = iter->attribute("acceleration").as_float();
			config.fAngSpeed = iter->attribute("angularSpeed").as_float();
			config.fProjSpeed = iter->attribute("projectileSpeed").as_float();
			config.vShootHelper = iter->attribute("shootHelper").as_vector();
			config.fShootCooldown = iter->attribute("shootCooldown").as_float();
			config.numShotsInBurst = iter->attribute("shotsInBurst").as_int();
			config.fBurstCooldown = iter->attribute("burstCooldown").as_float();
			config.fProjectileLifetime = iter->attribute("projectileLifetime").as_float();
			config.ammoCount = iter->attribute("ammoCount").as_int();
			config.fFuel = iter->attribute("fuel").as_float();
			config.fConsumption = iter->attribute("consumption").as_float();

			m_configurations.emplace(std::move(name), std::move(config));
		}
	}
}

const CPlayerConfiguration::SPlayerConfiguration* CPlayerConfiguration::GetConfiguration(const std::string& name) const
{
	auto fnd = m_configurations.find(name);
	return fnd != m_configurations.end() ? &fnd->second : nullptr;
}

static std::string ReturnDefault = "";

const std::string& CPlayerConfiguration::GetDefaultConfiguration() const
{
	return m_configurations.empty() ? ReturnDefault : m_configurations.begin()->first;
}

const std::string& CPlayerConfiguration::GetNextConfiguration(const std::string& current) const
{
	auto fnd = m_configurations.find(current);
	if (fnd != m_configurations.end())
	{
		if (++fnd != m_configurations.end())
		{
			return fnd->first;
		}
		else
		{
			return m_configurations.begin()->first;
		}
	}
	return ReturnDefault;
}

const std::string& CPlayerConfiguration::GetPreviousConfiguration(const std::string& current) const
{
	auto fnd = m_configurations.find(current);
	if (fnd != m_configurations.end())
	{
		if (--fnd != m_configurations.end())
		{
			return fnd->first;
		}
		else
		{
			return m_configurations.rbegin()->first;
		}
	}
	return ReturnDefault;
}