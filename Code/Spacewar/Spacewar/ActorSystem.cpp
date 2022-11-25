#include "ActorSystem.h"
#include "Player.h"

void CActorSystem::RemoveActor(SmartId sid, bool immediate)
{
	if (immediate)
	{
		m_actors.erase(sid);
	}
	else
	{
		m_removeDeferred.push_back(sid);
	}
}

CActor* CActorSystem::GetActor(SmartId sid)
{
	auto fnd = m_actors.find(sid);
	return fnd != m_actors.end() ? fnd->second.get() : nullptr;
}

void CActorSystem::ForEachPlayer(std::function<void(CPlayer*)> f)
{
	for (const auto& [sid, pActor] : m_actors)
	{
		if (pActor->GetType() == EActorType::Player)
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
		if (pActor->GetType() == EActorType::Player)
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

void CActorSystem::CollectGarbage()
{
	for (SmartId sid : m_removeDeferred)
	{
		m_actors.erase(sid);
	}
	m_removeDeferred.clear();
}

void CActorSystem::Release()
{
	m_removeDeferred.clear();
	m_actors.clear();
}