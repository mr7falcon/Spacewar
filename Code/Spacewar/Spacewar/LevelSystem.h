#pragma once

#include <vector>
#include <random>

#include <SFML\System\Time.hpp>

#include "LevelConfiguration.h"
#include "LogicalSystem.h"

class CLevelSystem
{
public:

	CLevelSystem() = default;
	CLevelSystem(const CLevelSystem&) = delete;

	void CreateLevel(const std::string& config);
	SmartId SpawnPlayer();

	void TeleportEntity(CLogicalEntity* pEntity, const sf::Vector2f& vOrg);

	void Update(sf::Time dt);

	float GetLevelSize() const { return m_pLevelConfig ? m_pLevelConfig->fSize : 0.f; }
	bool IsConsumablesAllowed() const { return m_pLevelConfig ? m_pLevelConfig->bAllowConsumables : false; }

private:

	void GenerateStars();
	void SpawnStar(int textureId, const sf::Vector2f& vPos, const sf::Vector2f& vScale);
	void SpawnHole(const CLevelConfiguration::SHoleConfiguration& config);
	void SpawnBonus(const CLevelConfiguration::SBonusConfiguration& config);

	void ScheduleBonus();

	float RandFloat();
	int RandInt(int min, int max);

private:

	std::mt19937 m_randomEngine;

	const CLevelConfiguration::SConfiguration* m_pLevelConfig = nullptr;

	std::vector<SmartId> m_playerSpawners;

	float m_fNextBonusCooldown = 0.f;
	float m_fNextHoleCooldown = 0.f;
};