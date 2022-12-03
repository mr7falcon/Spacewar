#include <iostream>

#include "Player.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "ConfigurationSystem.h"
#include "LevelSystem.h"
#include "PhysicalEntity.h"
#include "Projectile.h"
#include "FeedbackSystem.h"

CPlayer::CPlayer(const std::string& configName, const CPlayerConfiguration::SPlayerConfiguration* pConfig)
	: CActor(pConfig->entityName), m_configName(configName), m_pConfig(pConfig)
{
	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsConsumablesAllowed())
	{
		m_dShotsInBurst = m_pConfig->dNumShotsInBurst;
		m_dAmmoCount = m_pConfig->dAmmoCount;
		m_fFuel = m_pConfig->fFuel;
	}
	SetNeedSerialize();
}

CPlayer::~CPlayer()
{
	if (m_pController)
	{
		m_pController->UnregisterEventListener(this);
	}
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->OnPlayerDestroyed(m_entityId, m_dScore);
}

void CPlayer::SetController(const std::shared_ptr<IController>& pController)
{
	if (m_pController)
	{
		m_pController->UnregisterEventListener(this);
	}
	m_pController = pController;
	if (m_pController)
	{
		m_pController->RegisterEventListener(this);
	}
}

void CPlayer::SetShooting(bool bShooting)
{
	m_bShooting = bShooting;
}

void CPlayer::SetAmmoCount(int dAmmoCount)
{
	m_dAmmoCount = dAmmoCount;
	SetNeedSerialize();
}

void CPlayer::SetFuel(float fFuel)
{
	m_fFuel = fFuel;
	SetNeedSerialize();
}

void CPlayer::SetScore(int dScore)
{
	m_dScore = dScore;
	SetNeedSerialize();
}

void CPlayer::OnControllerEvent(EControllerEvent evt)
{
	if (!CGame::Get().IsServer() || !CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		return;
	}

	CFeedbackSystem* pFeedbackSystem = CGame::Get().GetLogicalSystem()->GetFeedbackSystem();
	CLogicalEntity* pEntity = GetEntity();

	bool bChanged = false;
	
	switch (evt)
	{
	case EControllerEvent_MoveForward_Pressed:
		bChanged = m_fAccel != m_pConfig->fAccel;
		m_fAccel = m_pConfig->fAccel;
		pFeedbackSystem->OnEvent(m_entityId, m_pConfig->feedbackSchema, CFeedbackConfiguration::Move);
		break;
	case EControllerEvent_MoveForward_Released:
		bChanged = m_fAccel != 0.f;
		m_fAccel = 0.f;
		pFeedbackSystem->OnEventEnd(m_entityId, CFeedbackConfiguration::Move);
		break;
	case EControllerEvent_RotatePositive_Pressed:
		bChanged = pEntity->GetAngularSpeed() != m_pConfig->fAngSpeed;
		pEntity->SetAngularSpeed(m_pConfig->fAngSpeed);
		break;
	case EControllerEvent_Rotate_Released:
		bChanged = pEntity->GetAngularSpeed() != 0.f;
		pEntity->SetAngularSpeed(0.f);
		break;
	case EControllerEvent_RotateNegative_Pressed:
		bChanged = pEntity->GetAngularSpeed() != -m_pConfig->fAngSpeed;
		pEntity->SetAngularSpeed(-m_pConfig->fAngSpeed);
		break;
	case EControllerEvent_Shoot_Pressed:
		bChanged = !m_bShooting;
		SetShooting(true);
		break;
	case EControllerEvent_Shoot_Released:
		bChanged = m_bShooting;
		SetShooting(false);
		break;
	}

	if (bChanged)
	{
		SetNeedSerialize();
	}
}

void CPlayer::OnCollision(SmartId sid)
{
	if (CActor* pActor = CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid))
	{
		if (pActor->GetType() == EActorType_Projectile)
		{
			SmartId ownerId = static_cast<CProjectile*>(pActor)->GetOwnerId();
			if (ownerId != m_entityId)
			{
				CGame::Get().GetLogicalSystem()->GetFeedbackSystem()->OnEvent(m_entityId, m_pConfig->feedbackSchema, CFeedbackConfiguration::Death);
				Destroy();

				if (CPlayer* pOwner = static_cast<CPlayer*>(CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(ownerId)))
				{
					pOwner->SetScore(pOwner->GetScore() + 1);
				}
			}
		}
		else if (pActor->GetType() == EActorType_Hole)
		{
			SetNeedSerialize();
		}
	}
}

void CPlayer::Update(sf::Time dt)
{
	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		if (m_pController)
		{
			m_pController->Update();
		}
	}

	if (m_fAccel > 0.f && m_fFuel != 0.f)
	{
		CLogicalEntity* pEntity = GetEntity();
		sf::Vector2f vel = pEntity->GetVelocity();
		vel += pEntity->GetForwardDirection() * m_fAccel * dt.asSeconds();
		pEntity->SetVelocity(vel);

		if (m_fFuel > 0.f)
		{
			m_fFuel = std::max(0.f, m_fFuel - m_pConfig->fConsumption * dt.asSeconds());
			SetNeedSerialize();
		}
	}

	m_fShotsCooldown -= dt.asSeconds();
	m_fBurstCooldown -= dt.asSeconds();

	if (m_fBurstCooldown <= 0.f)
	{
		m_dShotsInBurst = m_pConfig->dNumShotsInBurst;
	}

	if (m_bShooting && CanShoot())
	{
		Shoot();
	}
}

void CPlayer::Serialize(sf::Packet& packet, uint8_t mode, uint16_t& size)
{
	CActor::Serialize(packet, mode, size);

	float fAccel = m_fAccel;
	sf::Int16 dNumShotsInBurst = m_dShotsInBurst;
	sf::Int16 dAmmoCount = m_dAmmoCount;
	float fFuel = m_fFuel;
	sf::Int16 dScore = m_dScore;

	SerializeParameters(packet, mode, size, fAccel, dNumShotsInBurst, dAmmoCount, fFuel, dScore);

	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		m_fAccel = fAccel;
		m_dShotsInBurst = dNumShotsInBurst;
		m_dAmmoCount = dAmmoCount;
		m_fFuel = fFuel;
		m_dScore = dScore;
	}
}

bool CPlayer::CanShoot() const
{
	return m_dShotsInBurst > 0 && m_fShotsCooldown <= 0.f && m_dAmmoCount != 0;
}

void CPlayer::Shoot()
{
	if (!CGame::Get().IsServer() && CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		return;
	}

	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	SmartId projectileId = pActorSystem->CreateActor<CProjectile>(m_pConfig->projectileEntityName);
	
	if (CProjectile* pProjectile = static_cast<CProjectile*>(pActorSystem->GetActor(projectileId)))
	{
		CLogicalEntity* pEntity = GetEntity();
		
		pProjectile->SetLifetime(m_pConfig->fProjectileLifetime);
		pProjectile->SetOwnerId(m_entityId);

		if (CLogicalEntity* pProjectileEntity = pProjectile->GetEntity())
		{
			pProjectileEntity->SetPosition(pEntity->GetTransform().transformPoint(m_pConfig->vShootHelper));
			pProjectileEntity->SetRotation(pEntity->GetRotation());
			pProjectileEntity->SetVelocity(pEntity->GetForwardDirection() * m_pConfig->fProjSpeed);
		}
	}

	--m_dShotsInBurst;
	m_fShotsCooldown = m_pConfig->fShootCooldown;
	m_fBurstCooldown = m_pConfig->fBurstCooldown;
	
	if (m_dAmmoCount > 0)
	{
		--m_dAmmoCount;
	}
	
	SetNeedSerialize();

	CGame::Get().GetLogicalSystem()->GetFeedbackSystem()->OnEvent(m_entityId, m_pConfig->feedbackSchema, CFeedbackConfiguration::Shoot);
}