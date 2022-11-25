#include "Player.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ConfigurationSystem.h"

CPlayer::CPlayer()
	: CActor("Spaceship")
{}

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
	case EControllerEvent_Shoot:
		break;
	}
}

void CPlayer::Update(sf::Time dt)
{
	if (m_fAccel > 0.f)
	{
		CLogicalEntity* pEntity = GetEntity();
		sf::Vector2f vel = pEntity->GetVelocity();
		vel += pEntity->GetForwardDirection() * m_fAccel * dt.asSeconds();
		pEntity->SetVelocity(vel);
	}
}