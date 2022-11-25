#include "ActorSystem.h"
#include "Player.h"

CActor* CActorSystem::GetActor(SmartId sid)
{
	auto fnd = m_actors.find(sid);
	return fnd != m_actors.end() ? fnd->second.get() : nullptr;
}

void CActorSystem::ForEachPlayer(std::function<void(CPlayer*)> f)
{
	for (const auto& [sid, pActor] : m_actors)
	{
		if (pActor->IsPlayer())
		{
			f(static_cast<CPlayer*>(pActor.get()));
		}
	}
}

int CActorSystem::GetNumPlayers() const
{
	int count = 0;
	for (const auto& [sid, pActor] : m_actors)
	{
		if (pActor->IsPlayer())
		{
			++count;
		}
	}
	return count;
}

void CActorSystem::Update(sf::Time dt)
{
	for (const auto& [sid, pActor] : m_actors)
	{
		pActor->Update(dt);
	}
}