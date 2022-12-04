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
	CGame::Get().GetLogicalSystem()->GetFeedbackSystem()->DestroyEvents(m_entityId);
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
	if (m_bShooting != bShooting)
	{
		m_bShooting = bShooting;
		SetNeedSerialize();
	}
}

void CPlayer::SetAcceleration(float fAccel)
{
	CFeedbackSystem* pFeedbackSystem = CGame::Get().GetLogicalSystem()->GetFeedbackSystem();

	if (m_fAccel == 0.f && fAccel > 0.f)
	{
		pFeedbackSystem->OnEvent(m_entityId, m_pConfig->feedbackSchema, CFeedbackConfiguration::Move);
		SetNeedSerialize();
	}
	else if (m_fAccel > 0.f && fAccel == 0.f)
	{
		pFeedbackSystem->OnEventEnd(m_entityId, CFeedbackConfiguration::Move);
		SetNeedSerialize();
	}

	m_fAccel = fAccel;
}

void CPlayer::SetAngularSpeed(float fAngSpeed)
{
	CLogicalEntity* pEntity = GetEntity();
	if (pEntity->GetAngularSpeed() != fAngSpeed)
	{
		pEntity->SetAngularSpeed(fAngSpeed);
		SetNeedSerialize();
	}
}

void CPlayer::SetAmmoCount(int dAmmoCount)
{
	if (m_dAmmoCount != dAmmoCount)
	{
		m_dAmmoCount = dAmmoCount;
		SetNeedSerialize();
	}
}

void CPlayer::SetShotsInBurst(int dShotsInBurst)
{
	if (!CGame::Get().IsServer() && dShotsInBurst < m_dShotsInBurst)
	{
		CGame::Get().GetLogicalSystem()->GetFeedbackSystem()->OnEvent(m_entityId, m_pConfig->feedbackSchema, CFeedbackConfiguration::Shoot);
	}
	m_dShotsInBurst = dShotsInBurst;
}

void CPlayer::SetFuel(float fFuel)
{
	if (m_fFuel != fFuel)
	{
		m_fFuel = fFuel;
		SetNeedSerialize();
	}
}

void CPlayer::SetScore(int dScore)
{
	if (m_dScore != dScore)
	{
		m_dScore = dScore;
		SetNeedSerialize();
	}
}

void CPlayer::OnControllerEvent(EControllerEvent evt)
{
	if (!CGame::Get().IsServer() || !CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		return;
	}

	switch (evt)
	{
	case EControllerEvent_MoveForward_Pressed:
		SetAcceleration(m_pConfig->fAccel);
		break;
	case EControllerEvent_MoveForward_Released:
		SetAcceleration(0.f);
		break;
	case EControllerEvent_RotatePositive_Pressed:
		SetAngularSpeed(m_pConfig->fAngSpeed);
		break;
	case EControllerEvent_Rotate_Released:
		SetAngularSpeed(0.f);
		break;
	case EControllerEvent_RotateNegative_Pressed:
		SetAngularSpeed(-m_pConfig->fAngSpeed);
		break;
	case EControllerEvent_Shoot_Pressed:
		SetShooting(true);
		break;
	case EControllerEvent_Shoot_Released:
		SetShooting(false);
		break;
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

	if (CGame::Get().IsServer() || !CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		if (m_fBurstCooldown <= 0.f)
		{
			m_dShotsInBurst = m_pConfig->dNumShotsInBurst;
			SetNeedSerialize();
		}
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
	sf::Int16 dAmmoCount = m_dAmmoCount;
	sf::Int16 dShotsInBurst = m_dShotsInBurst;
	float fFuel = m_fFuel;
	sf::Int16 dScore = m_dScore;

	SerializeParameters(packet, mode, size, fAccel, dAmmoCount, dShotsInBurst, fFuel, dScore);

	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		SetAcceleration(fAccel);
		SetFuel(fFuel);
		SetScore(dScore);
		SetAmmoCount(dAmmoCount);
		SetShotsInBurst(dShotsInBurst);
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