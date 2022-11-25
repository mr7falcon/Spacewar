#include "Projectile.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"

CProjectile::CProjectile() : CActor("Projectile") {}

void CProjectile::OnCollision(SmartId sid)
{
	if (CActor* pActor = CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid))
	{
		if (pActor->GetType() == EActorType::Player)
		{
			Destroy();
		}
	}
}

void CProjectile::Update(sf::Time dt)
{
	m_fLifetime -= dt.asSeconds();
	if (m_fLifetime <= 0.f)
	{
		Destroy();
	}
}