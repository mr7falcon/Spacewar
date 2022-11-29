#pragma once

#include <map>
#include <vector>
#include <memory>
#include <functional>

#include <SFML/System/Time.hpp>

#include "Actor.h"

class CPlayer;

class CActorSystem
{
public:

	CActorSystem() = default;
	CActorSystem(const CActorSystem&) = delete;
	
	template<typename T, typename... V>
	SmartId CreateActor(V&&... args)
	{
		std::unique_ptr<CActor> pActor = std::make_unique<T>(std::forward<V>(args)...);
		SmartId sid = pActor->GetEntity()->GetId();
		m_actors[sid] = std::move(pActor);
		return sid;
	}
	void RemoveActor(SmartId sid, bool immediate = false);

	CActor* GetActor(SmartId sid);

	void ForEachPlayer(std::function<void(CPlayer*)> f);
	int GetNumPlayers() const;
	SmartId GetFirstPlayerId() const;
	SmartId GetLastPlayerId() const;

	void Update(sf::Time dt);
	void CollectGarbage();
	void Release();

private:
	
	std::map<SmartId, std::unique_ptr<CActor>> m_actors;
	std::vector<SmartId> m_removeDeferred;
};