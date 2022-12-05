#pragma once

#include "LogicalSystem.h"
#include "ConfigurationSystem/LevelConfiguration.h"
#include "Controllers/Controller.h"

#include <vector>
#include <memory>
#include <random>

#include <SFML\System\Time.hpp>

/**
 * @class CLevelSystem
 * Level system manages levels and all the objects on them
 * including players, holes, stars, etc. Level system is
 * responsible for the level generation, loading and specific
 * game rules logic like players' or bonuses' spawning.
 * Levels are also configured in the xml (see CLevelConfiguration).
 */
class CLevelSystem
{
public:

	CLevelSystem() = default;
	CLevelSystem(const CLevelSystem&) = delete;

	/**
	 * @function CreateLevel
	 * Create level from the specified configuration.
	 * This function generates stars, places holes players
	 * and loads a specific layout.
	 * 
	 * @param config - level configuration name.
	 */
	void CreateLevel(const std::string& config);

	/**
	 * @function CreateDefaultLevel
	 * Create "Menu" level.
	 */
	void CreateDefaultLevel();

	/**
	 * @function ClearLevel
	 * Clear logical, render, physics and sound systems.
	 */
	void ClearLevel();

	/**
	 * @function SpawnPlayer
	 * Spawn player from the specified configuration.
	 * The new player could be spawned only if there are any
	 * of the level spawners is available. This function also
	 * loads the player layout correspinding to the affected spawner.
	 * 
	 * @param config - player configuration name.
	 * @param pController - controller to link with the new player
	 * @return SmartId of the new spawned player's entity
	 */
	SmartId SpawnPlayer(const std::string& config, const std::shared_ptr<CController>& pController = nullptr);

	/**
	 * @function OnPlayerDestroyed
	 * Free the corresponding spawner and save the dead player
	 * information for the revive.
	 * 
	 * @param sid - SmartId of the destroyed player's entity.
	 * @param score - player's score to save.
	 */
	void OnPlayerDestroyed(SmartId sid, int score);

	/**
	 * @function DeletePlayer
	 * Complete deletion the player with the specified id from the level.
	 * Clears all the information about the player and unloads the player's
	 * layout. Deleted players are not revived anyore.
	 * 
	 * @param sid - SmartId of the player to be deleted.
	 */
	void DeletePlayer(SmartId sid);

	/**
	 * @function ReloadPlayersLayouts
	 * Load or unload player layouts if there are some changes in the players composition.
	 */
	void ReloadPlayersLayouts();

	/**
	 * @function ReloadGlobalLayout
	 * Reload the main level layout.
	 */
	void ReloadGlobalLayout();

	/**
	 * @function TeleportEntity
	 * Teleport the specified entity in the random point on the map
	 * on the configured distance from the origin (used for the holes).
	 * 
	 * @param pEntity - entity to be teleported.
	 * @param vOrg - teleportation's origin.
	 */
	void TeleportEntity(CLogicalEntity* pEntity, const sf::Vector2f& vOrg);

	/**
	 * @function Update
	 * Function to update the level state. Should be called each frame.
	 *
	 * @param dt - delta time since the last function call.
	 */
	void Update(sf::Time dt);

	/**
	 * @function GetLevelSize
	 * Get the level size in the world coordinates. The level is always squared.
	 * 
	 * @return Lenght of the level's square side.
	 */
	float GetLevelSize() const { return m_pLevelConfig ? m_pLevelConfig->fSize : 0.f; }

	/**
	 * @function IsConsumablesAllowed
	 * Check if the consumables are allowed for this level.
	 * 
	 * @return True if the level supports the consumables, false otherwise.
	 */
	bool IsConsumablesAllowed() const { return m_pLevelConfig ? m_pLevelConfig->bAllowConsumables : false; }

	/**
	 * @function IsInGame
	 * Check if the current level is a game level.
	 * 
	 * @return True if the current level is a game level, false otherwise.
	 */
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