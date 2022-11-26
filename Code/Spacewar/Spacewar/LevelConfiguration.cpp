#include <iostream>

#include <pugixml.hpp>

#include "LevelConfiguration.h"

CLevelConfiguration::CLevelConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		std::cout << "Levels configuration loading failed: " << res.description() << std::endl;
		return;
	}

	auto root = doc.child("Levels");
	if (!root)
	{
		std::cout << "Invalid root element in levels configuration" << std::endl;
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
				std::cout << "Level configuration with name " << name << " already exists" << std::endl;
				continue;
			}

			SConfiguration config;

			config.fSize = iter->attribute("size").as_float();
			config.bAllowConsumables = iter->attribute("allowConsumables").as_bool();

			auto holes = iter->child("Holes");
			if (holes)
			{
				config.holes = ParseHoles(holes);
			}

			auto playerSpawners = iter->child("PlayerSpawners");
			if (playerSpawners)
			{
				for (auto spawnerIter = playerSpawners.begin(); spawnerIter != playerSpawners.end(); ++spawnerIter)
				{
					config.playerSpawners.push_back(ParsePlayerSpawner(*spawnerIter));
				}
			}

			auto stars = iter->child("Stars");
			if (stars)
			{
				config.stars = ParseStars(stars);
			}

			auto bonuses = iter->child("Bonuses");
			if (bonuses)
			{
				config.bonuses = ParseBonuses(bonuses);
			}

			m_configurations[std::move(name)] = std::move(config);
		}
	}
}

CLevelConfiguration::SHolesConfiguration CLevelConfiguration::ParseHoles(const pugi::xml_node& node)
{
	SHolesConfiguration config;
	config.dNumDynamicHoles = node.attribute("dynamicHoles").as_int();
	config.fMinTeleportRange = node.attribute("minTeleportRange").as_float();
	config.fAngSpeed = node.attribute("rotationSpeed").as_float();
	for (auto iter = node.begin(); iter != node.end(); ++iter)
	{
		config.staticHoles.push_back(ParseHole(*iter));
	}
	return config;
}

CLevelConfiguration::SHoleConfiguration CLevelConfiguration::ParseHole(const pugi::xml_node& node)
{
	SHoleConfiguration config;
	config.vPos = node.attribute("position").as_vector();
	config.fGravity = node.attribute("gravity").as_float();
	config.vVel = node.attribute("velocity").as_vector();
	return config;
}

CLevelConfiguration::SPlayerSpawnerConfiguration CLevelConfiguration::ParsePlayerSpawner(const pugi::xml_node& node)
{
	SPlayerSpawnerConfiguration config;
	config.vPos = node.attribute("position").as_vector();
	config.fRot = node.attribute("rotation").as_float();
	return config;
}

CLevelConfiguration::SStarsConfiguration CLevelConfiguration::ParseStars(const pugi::xml_node& node)
{
	SStarsConfiguration config;
	config.dNumPlacesInRow = node.attribute("rowPlaces").as_int();
	config.fAvailableSpace = node.attribute("availableSpace").as_float();
	config.fSpawnProbability = node.attribute("spawnProbability").as_float();
	config.fScaleMin = node.attribute("scaleMin").as_float();
	config.fScaleMax = node.attribute("scaleMax").as_float();
	if (config.fScaleMin > config.fScaleMax)
	{
		std::swap(config.fScaleMin, config.fScaleMax);
	}
	return config;
}

inline static CBonus::EBonusType ParseBonusType(const std::string& type)
{
	if (type == "ammo")
		return CBonus::Ammo;
	if (type == "fuel")
		return CBonus::Fuel;
	return CBonus::None;
}

CLevelConfiguration::SBonusesConfiguration CLevelConfiguration::ParseBonuses(const pugi::xml_node& node)
{
	SBonusesConfiguration config;
	config.fCooldownMin = node.attribute("cooldownMin").as_float();
	config.fCooldownMax = node.attribute("cooldownMax").as_float();
	for (auto iter = node.begin(); iter != node.end(); ++iter)
	{
		config.bonuses.push_back(ParseBonus(*iter));
	}
	return config;
}

CLevelConfiguration::SBonusConfiguration CLevelConfiguration::ParseBonus(const pugi::xml_node& node)
{
	SBonusConfiguration config;
	config.type = ParseBonusType(node.attribute("type").value());
	config.fVal = node.attribute("value").as_float();
	config.fLifetime = node.attribute("lifetime").as_float();
	config.entityName = node.attribute("entity").value();
	return config;
}

const CLevelConfiguration::SConfiguration* CLevelConfiguration::GetConfiguration(const std::string& name) const
{
	auto fnd = m_configurations.find(name);
	return fnd != m_configurations.end() ? &fnd->second : nullptr;
}