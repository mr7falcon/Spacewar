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

			config.vSize = iter->attribute("size").as_vector();

			auto holes = iter->child("Holes");
			if (holes)
			{
				config.dNumDynamicHoles = holes.attribute("dynamicHoles").as_int();
				for (auto holeIter = holes.begin(); holeIter != holes.end(); ++holeIter)
				{
					config.staticHoles.push_back(ParseHole(*holeIter));
				}
			}

			auto playerSpawners = iter->child("PlayerSpawners");
			if (playerSpawners)
			{
				for (auto spawnerIter = playerSpawners.begin(); spawnerIter != playerSpawners.end(); ++spawnerIter)
				{
					config.playerSpawners.push_back(ParsePlayerSpawner(*spawnerIter));
				}
			}

			m_configurations[std::move(name)] = std::move(config);
		}
	}
}

CLevelConfiguration::SHoleConfiguration CLevelConfiguration::ParseHole(const pugi::xml_node& node)
{
	SHoleConfiguration config;
	config.vPos = node.attribute("position").as_vector();
	config.fGravity = node.attribute("gravity").as_float();
	config.vVel = node.attribute("velocity").as_vector();
	config.fAngSpeed = node.attribute("rotationSpeed").as_float();
	return config;
}

CLevelConfiguration::SPlayerSpawnerConfiguration CLevelConfiguration::ParsePlayerSpawner(const pugi::xml_node& node)
{
	SPlayerSpawnerConfiguration config;
	config.vPos = node.attribute("position").as_vector();
	config.fRot = node.attribute("rotation").as_float();
	return config;
}

const CLevelConfiguration::SConfiguration* CLevelConfiguration::GetConfiguration(const std::string& name) const
{
	auto fnd = m_configurations.find(name);
	return fnd != m_configurations.end() ? &fnd->second : nullptr;
}