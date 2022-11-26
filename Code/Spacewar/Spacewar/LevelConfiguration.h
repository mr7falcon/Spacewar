#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <SFML\System\Vector2.hpp>
#include <pugixml.hpp>

#include "Bonus.h"

class CLevelConfiguration
{
public:

	struct SStarsConfiguration
	{
		int dNumPlacesInRow = 0;
		float fAvailableSpace = 0.f;
		float fSpawnProbability = 0.f;
		float fScaleMin = 0.f;
		float fScaleMax = 0.f;
	};

	struct SHoleConfiguration
	{
		sf::Vector2f vPos;
		float fGravity = 0.f;
		sf::Vector2f vVel;
	};

	struct SHolesConfiguration
	{
		int dNumDynamicHoles = 0;
		float fMinTeleportRange = 0.f;
		float fAngSpeed = 0.f;
		std::vector<SHoleConfiguration> staticHoles;
	};

	struct SPlayerSpawnerConfiguration
	{
		sf::Vector2f vPos;
		float fRot = 0.f;
	};

	struct SBonusConfiguration
	{
		CBonus::EBonusType type = CBonus::None;
		float fVal = 0.f;
		float fLifetime = 0.f;
		std::string entityName;
	};

	struct SBonusesConfiguration
	{
		float fCooldownMin = 0.f;
		float fCooldownMax = 0.f;
		std::vector<SBonusConfiguration> bonuses;
	};

	struct SConfiguration
	{
		float fSize = 0.f;
		bool bAllowConsumables = false;
		SStarsConfiguration stars;
		SHolesConfiguration holes;
		std::vector<SPlayerSpawnerConfiguration> playerSpawners;
		SBonusesConfiguration bonuses;
	};

	CLevelConfiguration(const std::filesystem::path& path);
	CLevelConfiguration(const CLevelConfiguration&) = delete;

	const SConfiguration* GetConfiguration(const std::string& name) const;

private:

	SHolesConfiguration ParseHoles(const pugi::xml_node& node);
	SHoleConfiguration ParseHole(const pugi::xml_node& node);
	SPlayerSpawnerConfiguration ParsePlayerSpawner(const pugi::xml_node& node);
	SStarsConfiguration ParseStars(const pugi::xml_node& node);
	SBonusesConfiguration ParseBonuses(const pugi::xml_node& node);
	SBonusConfiguration ParseBonus(const pugi::xml_node& node);

private:

	std::map<std::string, SConfiguration> m_configurations;
};