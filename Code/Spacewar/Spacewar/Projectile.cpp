#include "Projectile.h"
#include "Game.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"

CProjectile::CProjectile(const std::string& entity) : CActor(entity) {}

void CProjectile::OnCollision(SmartId sid)
{
	if (CActor* pActor = CGame::Get().GetLogicalSystem()->GetActorSystem()->GetActor(sid))
	{
		if (pActor->GetType() == EActorType_Player && pActor->GetEntityId() != m_owner)
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

void CProjectile::SetLifetime(float fLifetime)
{
	m_fLifetime = fLifetime;
}

void CProjectile::SetOwnerId(SmartId sid)
{
	m_owner = sid;
}