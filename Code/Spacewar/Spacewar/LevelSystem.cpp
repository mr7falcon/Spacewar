#include <iostream>
#include <time.h>

#include "LevelSystem.h"
#include "ConfigurationSystem.h"
#include "ResourceSystem.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "RenderSystem.h"
#include "RenderProxy.h"
#include "ActorSystem.h"
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
	m_pLevelConfig = CGame::Get().GetConfigurationSystem()->GetLevelConfiguration()->GetConfiguration(config);
	if (!m_pLevelConfig)
	{
		std::cout << "Invalid level configuration " << config << std::endl;
		return;
	}

	m_randomEngine.seed((unsigned int)time(nullptr));

	GenerateStars();
	
	for (const auto& hole : m_pLevelConfig->holes.staticHoles)
	{
		SpawnHole(hole);
	}

	if (m_pLevelConfig->bAllowConsumables)
	{
		ScheduleBonus();
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
	SmartId renderEntityId = CGame::Get().GetRenderSystem()->CreateEntity();
	CRenderProxy* pRenderProxy = CGame::Get().GetRenderProxy();
	pRenderProxy->OnCommand<CRenderProxy::ERenderCommand_SetTexture>(renderEntityId, textureId);
	pRenderProxy->OnCommand<CRenderProxy::ERenderCommand_SetTransform>(renderEntityId, sf::Transform().translate(vPos).scale(vScale));
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
	if (m_pLevelConfig->bAllowConsumables && m_fNextBonusCooldown <= 0.f)
	{
		SpawnBonus(m_pLevelConfig->bonuses.bonuses[RandInt(0, m_pLevelConfig->bonuses.bonuses.size() - 1)]);
	}
}

void CLevelSystem::TeleportEntity(CLogicalEntity* pEntity, const sf::Vector2f& vOrg)
{
	if (!m_pLevelConfig)
	{
		return;
	}

	float availableSpaceSize = std::max(m_pLevelConfig->fSize - 2.f * m_pLevelConfig->holes.fMinTeleportRange, 0.f);
	sf::Vector2f vPos = vOrg + sf::Vector2f(m_pLevelConfig->holes.fMinTeleportRange + availableSpaceSize * RandFloat(), m_pLevelConfig->holes.fMinTeleportRange + availableSpaceSize * RandFloat());
	pEntity->SetPosition(vPos);

	const sf::Vector2f& vVel = pEntity->GetVelocity();
	float fRot = pEntity->GetRotation();
	float fNewRot = 360.f * RandFloat();
	pEntity->SetRotation(fNewRot);
	pEntity->SetVelocity(MathHelpers::RotateVector(vVel, fNewRot - fRot));
}