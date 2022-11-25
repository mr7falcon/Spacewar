#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <SFML\System\Vector2.hpp>
#include <pugixml.hpp>

class CLevelConfiguration
{
public:

	struct SHoleConfiguration
	{
		sf::Vector2f vPos;
		float fGravity = 0.f;
		sf::Vector2f vVel;
		float fAngSpeed = 0.f;
	};

	struct SPlayerSpawnerConfiguration
	{
		sf::Vector2f vPos;
		float fRot = 0.f;
	};

	struct SConfiguration
	{
		sf::Vector2f vSize;
		int dNumDynamicHoles = 0;
		std::vector<SHoleConfiguration> staticHoles;
		std::vector<SPlayerSpawnerConfiguration> playerSpawners;
	};

	CLevelConfiguration(const std::filesystem::path& path);
	CLevelConfiguration(const CLevelConfiguration&) = delete;

	const SConfiguration* GetConfiguration(const std::string& name) const;

private:

	SHoleConfiguration ParseHole(const pugi::xml_node& node);
	SPlayerSpawnerConfiguration ParsePlayerSpawner(const pugi::xml_node& node);

private:

	std::map<std::string, SConfiguration> m_configurations;
};