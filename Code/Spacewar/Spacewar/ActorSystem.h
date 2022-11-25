#pragma once

#include <map>
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
	
	template<typename T>
	SmartId CreateActor()
	{
		std::unique_ptr<CActor> pActor = std::make_unique<T>();
		SmartId sid = pActor->GetEntity()->GetId();
		m_actors[sid] = std::move(pActor);
		return sid;
	}

	CActor* GetActor(SmartId sid);

	void ForEachPlayer(std::function<void(CPlayer*)> f);
	int GetNumPlayers() const;

	void Update(sf::Time dt);

private:
	
	std::map<SmartId, std::unique_ptr<CActor>> m_actors;
};