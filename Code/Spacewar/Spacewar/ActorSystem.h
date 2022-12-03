#pragma once

#include <map>
#include <vector>
#include <memory>
#include <functional>

#include <SFML/System/Time.hpp>

#include "Hole.h"
#include "Projectile.h"
#include "Bonus.h"
#include "Player.h"
#include "Game.h"
#include "NetworkProxy.h"

template <typename>
struct is_player : std::false_type {};
template <>
struct is_player<CPlayer> : std::true_type {};

class CActorSystem
{
public:

	CActorSystem() = default;
	CActorSystem(const CActorSystem&) = delete;
	
	template<typename T, typename... V>
	SmartId CreateActor(V&&... args)
	{
		std::unique_ptr<CActor> pActor = std::make_unique<T>(std::forward<V>(args)...);
		SmartId sid = pActor->GetEntityId();
		m_actors[sid] = std::move(pActor);

		CGame::Get().GetNetworkProxy()->SendCreateActor(sid, m_actors[sid]->GetType(), std::forward<V>(args)...);

		if constexpr (is_player<T>::value)
		{
			static_cast<CPlayer*>(m_actors[sid].get())->SetShooting(m_bPlayersShooting);
		}

		return sid;
	}

	template<typename... V>
	SmartId CreateActor(EActorType type, V&&... args)
	{
		switch (type)
		{
		case EActorType_Hole:
			return CreateActor<CHole>(std::forward<V>(args)...);
		case EActorType_Bonus:
			return CreateActor<CBonus>(std::forward<V>(args)...);
		case EActorType_Projectile:
			return CreateActor<CProjectile>(std::forward<V>(args)...);
		}
		return InvalidLink;
	}

	void RemoveActor(SmartId sid, bool immediate = false);

	void SetPlayersShooting(bool bShooting);

	CActor* GetActor(SmartId sid);

	void ForEachPlayer(std::function<bool(CPlayer*)> f);
	int GetNumPlayers() const;
	SmartId GetFirstPlayerId() const;
	SmartId GetLastPlayerId() const;

	void Update(sf::Time dt);
	void CollectGarbage();
	void Release();
	void Serialize(sf::Packet& packet, bool bReading);

private:
	
	std::map<SmartId, std::unique_ptr<CActor>> m_actors;
	std::vector<SmartId> m_removeDeferred;

	bool m_bPlayersShooting = false;
};