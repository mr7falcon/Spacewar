#pragma once

#include <vector>

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\View.hpp>

#include "LevelConfiguration.h"
#include "EntitySystem.h"

class CLevelSystem
{
public:

	CLevelSystem() = default;
	CLevelSystem(const CLevelSystem&) = delete;

	void CreateLevel(const std::string& config);
	SmartId SpawnPlayer();

	void Update(sf::Time dt);

	const sf::Vector2f GetLevelSize() const { return m_pLevelConfig ? m_pLevelConfig->vSize : sf::Vector2f(); }

private:

	void GenerateStars();
	void SpawnHole(const CLevelConfiguration::SHoleConfiguration& config);

private:

	const CLevelConfiguration::SConfiguration* m_pLevelConfig = nullptr;
	
	sf::View m_view;

	std::vector<SmartId> m_playerSpawners;
};