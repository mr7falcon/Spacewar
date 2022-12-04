#include "StdAfx.h"
#include "Bonus.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "Player.h"

CBonus::CBonus(const std::string& entity) : CActor(entity) {}

void CBonus::SetBonus(EBonusType type, float fVal)
{
	m_type = type;
	m_fVal = fVal;
}

void CBonus::SetLifetime(float fLifetime)
{
	m_fLifetime = fLifetime;
}

void CBonus::OnCollision(SmartId sid)
{
	if (CActor* pActor = CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid))
	{
		if (pActor->GetType() == EActorType_Player)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(pActor);
			switch (m_type)
			{
			case Ammo:
				pPlayer->SetAmmoCount(pPlayer->GetAmmoCount() + (int)m_fVal);
				break;
			case Fuel:
				pPlayer->SetFuel(pPlayer->GetFuel() + m_fVal);
				break;
			}

			Destroy();
		}
	}
}

void CBonus::Update(sf::Time dt)
{
	m_fLifetime -= dt.asSeconds();
	if (m_fLifetime < 0.f)
	{
		Destroy();
	}
}