#pragma once

#include "LogicalSystem.h"
#include "ConfigurationSystem/LevelConfiguration.h"
#include "Controllers/Controller.h"

#include <vector>
#include <memory>
#include <random>

#include <SFML\System\Time.hpp>

class CLevelSystem
{
public:

	CLevelSystem() = default;
	CLevelSystem(const CLevelSystem&) = delete;

	void CreateLevel(const std::string& config);
	void CreateDefaultLevel();
	void ClearLevel();
	SmartId SpawnPlayer(const std::string& config, const std::shared_ptr<CController>& pController = nullptr);
	void OnPlayerDestroyed(SmartId sid, int dScore);
	void DeletePlayer(SmartId sid);

	void ReloadPlayersLayouts();
	void ReloadGlobalLayout();

	void TeleportEntity(CLogicalEntity* pEntity, const sf::Vector2f& vOrg);

	void Update(sf::Time dt);

	float GetLevelSize() const { return m_pLevelConfig ? m_pLevelConfig->fSize : 0.f; }
	bool IsConsumablesAllowed() const { return m_pLevelConfig ? m_pLevelConfig->bAllowConsumables : false; }
	bool IsInGame() const { return m_pLevelConfig ? m_pLevelConfig->bGameLevel : false; }

private:

	void SavePlayersInfo();
	void RecoverPlayers();

	void GenerateStars();
	void SpawnStar(int textureId, const sf::Vector2f& vPos, const sf::Vector2f& vScale);
	void SpawnHole(const CLevelConfiguration::SHoleConfiguration& config);
	void SpawnBonus(const CLevelConfiguration::SBonusConfiguration& config);

	void ScheduleBonus();
	void ScheduleRevive();

	float RandFloat();
	int RandInt(int min, int max);

private:

	std::mt19937 m_randomEngine;

	const CLevelConfiguration::SConfiguration* m_pLevelConfig = nullptr;

	std::vector<SmartId> m_playerSpawners;

	float m_fNextBonusCooldown = 0.f;
	float m_fNextHoleCooldown = 0.f;
	float m_fReviveCooldown = 0.f;

	struct SPlayerInfo
	{
		SmartId sid = InvalidLink;
		std::string config;
		std::shared_ptr<CController> controller;
		int dScore = 0;
	};
	std::vector<SPlayerInfo> m_savedPlayers;
};