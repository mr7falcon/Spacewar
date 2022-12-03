#include <iostream>
#include <time.h>

#include "LevelSystem.h"
#include "ConfigurationSystem.h"
#include "PlayerConfiguration.h"
#include "ResourceSystem.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "RenderSystem.h"
#include "RenderProxy.h"
#include "PhysicalSystem.h"
#include "NetworkProxy.h"
#include "ActorSystem.h"
#include "UISystem.h"
#include "Player.h"
#include "Hole.h"
#include "MathHelpers.h"

float CLevelSystem::RandFloat()
{
	static std::uniform_real_distribution<float> dist(0.f, 1.f);
	return dist(m_randomEngine);
}

int CLevelSystem::RandInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(m_randomEngine);
}

void CLevelSystem::CreateLevel(const std::string& config)
{
	CGame::Get().Pause(false);

	if (CGame::Get().IsServer() && !IsInGame())
	{
		SavePlayersInfo();
	}

	if (m_pLevelConfig)
	{
		ClearLevel();
	}

	m_pLevelConfig = CGame::Get().GetConfigurationSystem()->GetLevelConfiguration()->GetConfiguration(config);
	if (!m_pLevelConfig)
	{
		std::cout << "Invalid level configuration " << config << std::endl;
		return;
	}

	if (CGame::Get().IsServer())
	{
		CGame::Get().GetNetworkProxy()->BroadcastServerMessage<ServerMessage::SStartLevelMessage>(config);
	}

	m_playerSpawners.resize(m_pLevelConfig->playerSpawners.size());
	for (int i = 0; i < m_playerSpawners.size(); ++i)
	{
		m_playerSpawners[i] = InvalidLink;
	}

	ReloadGlobalLayout();

	m_randomEngine.seed((unsigned int)time(nullptr));

	GenerateStars();
	
	if (!CGame::Get().IsServer())
	{
		return;
	}

	RecoverPlayers();
	
	for (const auto& hole : m_pLevelConfig->holes.staticHoles)
	{
		SpawnHole(hole);
	}

	if (m_pLevelConfig->bAllowConsumables)
	{
		ScheduleBonus();
	}

}

void CLevelSystem::ClearLevel()
{
	CGame::Get().GetLogicalSystem()->Clear();
	CGame::Get().GetPhysicalSystem()->Clear();
	CGame::Get().GetRenderSystem()->Clear();
	CGame::Get().GetUISystem()->ResetLayout();
}

void CLevelSystem::SavePlayersInfo()
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->ForEachPlayer([this](CPlayer* pPlayer)
		{
			if (const auto& pController = pPlayer->GetController())
			{
				if (pController->GetType() != IController::Network || static_cast<CNetworkController*>(pController.get())->GetClientId() != -1)
				{
					SPlayerInfo info;
					info.config = pPlayer->GetConfigName();
					info.controller = pPlayer->GetController();
					m_savedPlayers.push_back(std::move(info));
				}
			}
			return true;
		});
}

void CLevelSystem::RecoverPlayers()
{
	for (const SPlayerInfo& playerInfo : m_savedPlayers)
	{
		SmartId sid = SpawnPlayer(playerInfo.config, playerInfo.controller);
	}

	if (!IsInGame())
	{
		m_savedPlayers.clear();
	}
}

void CLevelSystem::GenerateStars()
{
	if (!m_pLevelConfig || m_pLevelConfig->stars.dNumPlacesInRow == 0)
	{
		return;
	}

	auto textures = CGame::Get().GetResourceSystem()->GetTexturesInDirectory("Stars");
	if (textures.empty())
	{
		return;
	}

	const CLevelConfiguration::SStarsConfiguration config = m_pLevelConfig->stars;

	float cellSize = m_pLevelConfig->fSize / config.dNumPlacesInRow;
	for (int i = 0; i < config.dNumPlacesInRow; ++i)
	{
		for (int j = 0; j < config.dNumPlacesInRow; ++j)
		{
			float rnd = RandFloat();
			if (rnd < config.fSpawnProbability)
			{
				int texture = textures[RandInt(0, int(textures.size() - 1))];

				float rad = 0.5f * cellSize;
				sf::Vector2f vPos((float)i * cellSize + rad, (float)j * cellSize + rad);
				rad *= config.fAvailableSpace;
				vPos += sf::Vector2f(-rad + 2.f * rad * RandFloat(), -rad + 2.f * rad * RandFloat());

				float scale = config.fScaleMin + (config.fScaleMax - config.fScaleMin) * RandFloat();
				sf::Vector2f vScale(scale, scale);

				SpawnStar(texture, vPos, vScale);
			}
		}
	}
}

void CLevelSystem::SpawnStar(int textureId, const sf::Vector2f& vPos, const sf::Vector2f& vScale)
{
	CRenderSystem* pRenderSystem = CGame::Get().GetRenderSystem();
	SmartId renderEntityId = pRenderSystem->CreateEntity(CRenderEntity::Sprite);
	if (CRenderEntity* pEntity = pRenderSystem->GetEntity(renderEntityId))
	{
		pEntity->SetTransform(sf::Transform().translate(vPos).scale(vScale));
		CGame::Get().GetRenderProxy()->OnCommand<RenderCommand::SetTextureCommand>(renderEntityId, textureId);
	}
}

SmartId CLevelSystem::SpawnPlayer(const std::string& player, const std::shared_ptr<IController>& pController)
{
	if (!m_pLevelConfig)
	{
		return InvalidLink;
	}

	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();

	auto fnd = std::find(m_playerSpawners.begin(), m_playerSpawners.end(), InvalidLink);
	if (fnd == m_playerSpawners.end())
	{
		std::cout << "Maximum player count for this level reached" << std::endl;
		return InvalidLink;
	}

	size_t spawnerId = fnd - m_playerSpawners.begin();
	const CLevelConfiguration::SPlayerSpawnerConfiguration& config = m_pLevelConfig->playerSpawners[spawnerId];

	const auto* pPlayerConfig = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration()->GetConfiguration(player);
	if (!pPlayerConfig)
	{
		std::cout << "Failed to find player configuration " << player << std::endl;
		return InvalidLink;
	}

	SmartId playerId = pActorSystem->CreateActor<CPlayer>(player, pPlayerConfig);

	if (CPlayer* pPlayer = static_cast<CPlayer*>(pActorSystem->GetActor(playerId)))
	{
		if (CLogicalEntity* pPlayerEntity = pPlayer->GetEntity())
		{
			pPlayerEntity->SetPosition(config.vPos);
			pPlayerEntity->SetRotation(config.fRot);
		}
		pPlayer->SetController(pController);
	}

	m_playerSpawners[spawnerId] = playerId;

	CGame::Get().GetUISystem()->ReloadPlayerLayout(config.layout, playerId);
	
	return playerId;
}

void CLevelSystem::FreePlayerSpawner(SmartId sid)
{
	auto fnd = std::find(m_playerSpawners.begin(), m_playerSpawners.end(), sid);
	if (fnd != m_playerSpawners.end())
	{
		*fnd = InvalidLink;
	}

	ReloadPlayersLayouts();
}

void CLevelSystem::ReloadPlayersLayouts()
{
	if (!m_pLevelConfig)
	{
		return;
	}

	for (int i = 0; i < m_playerSpawners.size(); ++i)
	{
		CGame::Get().GetUISystem()->ReloadPlayerLayout(m_pLevelConfig->playerSpawners[i].layout, m_playerSpawners[i]);
	}
}

void CLevelSystem::ReloadGlobalLayout()
{
	if (m_pLevelConfig)
	{
		CGame::Get().GetUISystem()->LoadGlobalLayout(m_pLevelConfig->layout);
	}
}

void CLevelSystem::SpawnHole(const CLevelConfiguration::SHoleConfiguration& config)
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	SmartId holeId = pActorSystem->CreateActor<CHole>("Hole");
	
	if (CHole* pHole = static_cast<CHole*>(pActorSystem->GetActor(holeId)))
	{
		pHole->SetGravityForce(config.fGravity);

		if (CLogicalEntity* pHoleEntity = pHole->GetEntity())
		{
			pHoleEntity->SetPosition(config.vPos);
			if (m_pLevelConfig)
			{
				pHoleEntity->SetAngularSpeed(m_pLevelConfig->holes.fAngSpeed);
			}
		}
	}
}

void CLevelSystem::SpawnBonus(const CLevelConfiguration::SBonusConfiguration& config)
{
	if (!m_pLevelConfig)
	{
		return;
	}

	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	SmartId bonusId = pActorSystem->CreateActor<CBonus>(config.entityName);
	
	if (CBonus* pBonus = static_cast<CBonus*>(pActorSystem->GetActor(bonusId)))
	{
		pBonus->SetBonus(config.type, config.fVal);
		pBonus->SetLifetime(config.fLifetime);

		if (CLogicalEntity* pEntity = pBonus->GetEntity())
		{
			sf::Vector2f vPos(m_pLevelConfig->fSize * RandFloat(), m_pLevelConfig->fSize * RandFloat());
			pEntity->SetPosition(vPos);
		}
	}

	ScheduleBonus();
}

void CLevelSystem::ScheduleBonus()
{
	if (m_pLevelConfig)
	{
		m_fNextBonusCooldown = m_pLevelConfig->bonuses.fCooldownMin + (m_pLevelConfig->bonuses.fCooldownMax - m_pLevelConfig->bonuses.fCooldownMin) * RandFloat();
	}
}

void CLevelSystem::Update(sf::Time dt)
{
	if (!m_pLevelConfig)
	{
		return;
	}

	m_fNextBonusCooldown -= dt.asSeconds();
	if (CGame::Get().IsServer() && m_pLevelConfig->bAllowConsumables && m_fNextBonusCooldown <= 0.f)
	{
		SpawnBonus(m_pLevelConfig->bonuses.bonuses[RandInt(0, (int)(m_pLevelConfig->bonuses.bonuses.size() - 1))]);
	}
}

void CLevelSystem::TeleportEntity(CLogicalEntity* pEntity, const sf::Vector2f& vOrg)
{
	if (!CGame::Get().IsServer())
	{
		return;
	}

	if (!m_pLevelConfig)
	{
		return;
	}

	float availableSpaceSize = std::max(m_pLevelConfig->fSize - 2.f * m_pLevelConfig->holes.fMinTeleportRange, 0.f);
	sf::Vector2f vPos = vOrg + sf::Vector2f(m_pLevelConfig->holes.fMinTeleportRange + availableSpaceSize * RandFloat(), m_pLevelConfig->holes.fMinTeleportRange + availableSpaceSize * RandFloat());
	pEntity->SetPosition(vPos);

	float fRot = pEntity->GetRotation();
	float fNewRot = 360.f * RandFloat();
	pEntity->SetRotation(fNewRot);

	sf::Vector2f vDir = pEntity->GetForwardDirection();
	float fSpeed = m_pLevelConfig->holes.fMinAppearingSpeed + (m_pLevelConfig->holes.fMaxAppearingSpeed - m_pLevelConfig->holes.fMinAppearingSpeed) * RandFloat();
	pEntity->SetVelocity(MathHelpers::RotateVector(vDir, fNewRot - fRot) * fSpeed);
}