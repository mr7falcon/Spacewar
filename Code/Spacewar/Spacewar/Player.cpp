#include <iostream>

#include "Player.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "ConfigurationSystem.h"
#include "LevelSystem.h"
#include "PhysicalEntity.h"
#include "Projectile.h"

CPlayer::CPlayer(const std::string& configName, const CPlayerConfiguration::SPlayerConfiguration* pConfig)
	: CActor(pConfig->entityName), m_configName(configName), m_pConfig(pConfig)
{
	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsConsumablesAllowed())
	{
		m_dShotsInBurst = m_pConfig->dNumShotsInBurst;
		m_dAmmoCount = m_pConfig->dAmmoCount;
		m_fFuel = m_pConfig->fFuel;
	}
}

CPlayer::~CPlayer()
{
	if (m_pController)
	{
		m_pController->UnregisterEventListener(this);
	}
	CGame::Get().GetLogicalSystem()->GetLevelSystem()->FreePlayerSpawner(m_entityId);
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

void CPlayer::OnControllerEvent(EControllerEvent evt)
{
	if (!CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
	{
		return;
	}

	CLogicalEntity* pEntity = GetEntity();
	
	switch (evt)
	{
	case EControllerEvent_MoveForward_Pressed:
		m_fAccel = m_pConfig->fAccel;
		break;
	case EControllerEvent_MoveForward_Released:
		m_fAccel = 0.f;
		break;
	case EControllerEvent_RotatePositive_Pressed:
		pEntity->SetAngularSpeed(m_pConfig->fAngSpeed);
		break;
	case EControllerEvent_RotatePositive_Released:
	case EControllerEvent_RotateNegative_Released:
		pEntity->SetAngularSpeed(0.f);
		break;
	case EControllerEvent_RotateNegative_Pressed:
		pEntity->SetAngularSpeed(-m_pConfig->fAngSpeed);
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
		if (pActor->GetType() == EActorType::Projectile && static_cast<CProjectile*>(pActor)->GetOwnerId() != m_entityId)
		{
			Destroy();
		}
	}
}

void CPlayer::Update(sf::Time dt)
{
	if (m_fAccel > 0.f && m_fFuel != 0.f)
	{
		CLogicalEntity* pEntity = GetEntity();
		sf::Vector2f vel = pEntity->GetVelocity();
		vel += pEntity->GetForwardDirection() * m_fAccel * dt.asSeconds();
		pEntity->SetVelocity(vel);

		if (m_fFuel > 0.f)
		{
			m_fFuel = std::max(0.f, m_fFuel - m_pConfig->fConsumption * dt.asSeconds());
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

bool CPlayer::CanShoot() const
{
	return m_dShotsInBurst > 0 && m_fShotsCooldown <= 0.f;
}

void CPlayer::Shoot()
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	SmartId projectileId = pActorSystem->CreateActor<CProjectile>();
	
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
}