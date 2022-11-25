#include <iostream>

#include "Player.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "ConfigurationSystem.h"
#include "LevelSystem.h"
#include "PhysicalEntity.h"
#include "Projectile.h"

CPlayer::CPlayer()
	: CActor("Spaceship")
{
	if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsConsumablesAllowed())
	{
		const CConfigurationSystem::SPlayerConfiguration& config = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration();
		m_dAmmoCount = config.dAmmoCount;
		m_fFuel = config.fFuel;
	}
}

void CPlayer::SetController(std::unique_ptr<IController> pController)
{
	m_pController = std::move(pController);
	m_pController->SetControlledPlayer(this);
}

void CPlayer::OnControllerEvent(EControllerEvent evt)
{
	CLogicalEntity* pEntity = GetEntity();
	const CConfigurationSystem::SPlayerConfiguration& config = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration();

	switch (evt)
	{
	case EControllerEvent_MoveForward_Pressed:
		m_fAccel = config.fAccel;
		break;
	case EControllerEvent_MoveForward_Released:
		m_fAccel = 0.f;
		break;
	case EControllerEvent_RotatePositive_Pressed:
		pEntity->SetAngularSpeed(config.fAngSpeed);
		break;
	case EControllerEvent_RotatePositive_Released:
	case EControllerEvent_RotateNegative_Released:
		pEntity->SetAngularSpeed(0.f);
		break;
	case EControllerEvent_RotateNegative_Pressed:
		pEntity->SetAngularSpeed(-config.fAngSpeed);
		break;
	case EControllerEvent_Shoot_Pressed:
		m_bShooting = true;
		break;
	case EControllerEvent_Shoot_Released:
		m_bShooting = false;
		break;
	}
}

void CPlayer::OnCollision(SmartId sid)
{
	if (CActor* pActor = CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid))
	{
		if (pActor->GetType() == EActorType::Player || pActor->GetType() == EActorType::Projectile)
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
			const CConfigurationSystem::SPlayerConfiguration& config = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration();
			m_fFuel = std::max(0.f, m_fFuel - config.fConsumption * dt.asSeconds());
			std::cout << "Fuel " << m_fFuel << std::endl;
		}
	}

	if (m_bShooting && CanShoot())
	{
		Shoot();
	}
}

bool CPlayer::CanShoot() const
{
	const CConfigurationSystem::SPlayerConfiguration& config = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration();
	float elapsed = (CGame::Get().GetCurrentTime() - m_lastShootTime).asSeconds();
	return m_dAmmoCount != 0 && elapsed > config.fShootCooldown;
}

void CPlayer::Shoot()
{
	CActorSystem* pActorSystem = CGame::Get().GetLogicalSystem()->GetActorSystem();
	SmartId projectileId = pActorSystem->CreateActor<CProjectile>();
	
	if (CProjectile* pProjectile = static_cast<CProjectile*>(pActorSystem->GetActor(projectileId)))
	{
		CLogicalEntity* pEntity = GetEntity();
		const CConfigurationSystem::SPlayerConfiguration& config = CGame::Get().GetConfigurationSystem()->GetPlayerConfiguration();
		
		pProjectile->SetLifetime(config.fProjectileLifetime);

		if (CLogicalEntity* pProjectileEntity = pProjectile->GetEntity())
		{
			pProjectileEntity->SetPosition(pEntity->GetTransform().transformPoint(config.vShootHelper));
			pProjectileEntity->SetRotation(pEntity->GetRotation());
			pProjectileEntity->SetVelocity(pEntity->GetForwardDirection() * config.fProjSpeed);
		}
	}
	
	if (m_dAmmoCount > 0)
	{
		--m_dAmmoCount;
		std::cout << "Ammo count " << m_dAmmoCount << std::endl;
	}
	m_lastShootTime = CGame::Get().GetCurrentTime();
}