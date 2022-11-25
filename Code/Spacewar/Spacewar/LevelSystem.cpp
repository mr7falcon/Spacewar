#include <iostream>

#include "LevelSystem.h"
#include "ConfigurationSystem.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "Player.h"
#include "Hole.h"

void CLevelSystem::CreateLevel(const std::string& config)
{
	m_pLevelConfig = CGame::Get().GetConfigurationSystem()->GetLevelConfiguration()->GetConfiguration(config);
	if (!m_pLevelConfig)
	{
		std::cout << "Invalid level configuration " << config << std::endl;
		return;
	}

	m_view.reset(sf::FloatRect(0.f, 0.f, m_pLevelConfig->vSize.x, m_pLevelConfig->vSize.y));

	GenerateStars();
	
	for (const auto& hole : m_pLevelConfig->staticHoles)
	{
		SpawnHole(hole);
	}
}

void CLevelSystem::GenerateStars()
{

}

SmartId CLevelSystem::SpawnPlayer()
{
	if (!m_pLevelConfig)
	{
		return InvalidLink;
	}

	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();

	int numPlayers = pActorSystem->GetNumPlayers();
	if (numPlayers >= m_pLevelConfig->playerSpawners.size())
	{
		std::cout << "Maximum player count for this level reached" << std::endl;
		return InvalidLink;
	}

	SmartId playerId = pActorSystem->CreateActor<CPlayer>();

	if (CPlayer* pPlayer = static_cast<CPlayer*>(pActorSystem->GetActor(playerId)))
	{
		if (CLogicalEntity* pPlayerEntity = pPlayer->GetEntity())
		{
			const CLevelConfiguration::SPlayerSpawnerConfiguration& config = m_pLevelConfig->playerSpawners[numPlayers];
			pPlayerEntity->SetPosition(config.vPos);
			pPlayerEntity->SetRotation(config.fRot);
		}
	}
	
	return playerId;
}

void CLevelSystem::SpawnHole(const CLevelConfiguration::SHoleConfiguration& config)
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	SmartId holeId = pActorSystem->CreateActor<CHole>();
	if (CHole* pHole = static_cast<CHole*>(pActorSystem->GetActor(holeId)))
	{
		pHole->SetGravityForce(config.fGravity);
		if (CLogicalEntity* pHoleEntity = pHole->GetEntity())
		{
			pHoleEntity->SetPosition(config.vPos);
			pHoleEntity->SetVelocity(config.vVel);
			pHoleEntity->SetAngularSpeed(config.fAngSpeed);
		}
	}
}

void CLevelSystem::Update(sf::Time dt)
{

}