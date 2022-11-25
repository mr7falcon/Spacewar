#include <SFML/System/Vector2.hpp>

#include "Hole.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "LevelSystem.h"
#include "Player.h"
#include "MathHelpers.h"

CHole::CHole()
	: CActor("Hole")
{}

void CHole::OnCollision(SmartId sid)
{
	if (CActor* pActor = CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid))
	{
		if (pActor->GetType() == EActorType::Player)
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
		});
}