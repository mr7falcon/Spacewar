#include "StdAfx.h"
#include "Hole.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "LevelSystem.h"
#include "Player.h"
#include "MathHelpers.h"

CHole::CHole(const std::string& entity)
	: CActor(entity)
{}

void CHole::OnCollision(SmartId sid)
{
	if (CActor* pActor = CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid))
	{
		if (pActor->GetType() == EActorType_Player)
		{
			CGame::Get().GetLogicalSystem()->GetLevelSystem()->TeleportEntity(pActor->GetEntity(), GetEntity()->GetPosition());
		}
	}
}

void CHole::Update(sf::Time dt)
{
	CLogicalEntity* pEntity = GetEntity();

	CGame::Get().GetLogicalSystem()->GetActorSystem()->ForEachPlayer([&](CPlayer* pPlayer)
		{
			if (CLogicalEntity* pPlayerEntity = pPlayer->GetEntity())
			{
				sf::Vector2f dir = pEntity->GetPosition() - pPlayerEntity->GetPosition();
				float len = MathHelpers::GetLength(dir);
				if (len > 0.f)
				{
					dir *= m_fGravityForce / (len * len) * dt.asSeconds();
					pPlayerEntity->SetVelocity(pPlayerEntity->GetVelocity() + dir);
				}
			}
			return true;
		});
}

void CHole::Serialize(sf::Packet& packet, uint8_t mode, uint16_t& size)
{
	CActor::Serialize(packet, mode, size);

	float fGravity = m_fGravityForce;

	SerializeParameters(packet, mode, size, fGravity);

	m_fGravityForce = fGravity;
}

void CHole::SetGravityForce(float fGravity)
{
	m_fGravityForce = fGravity;
	SetNeedSerialize();
}