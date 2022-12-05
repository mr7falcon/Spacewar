#pragma once

#include "Hole.h"
#include "Projectile.h"
#include "Bonus.h"
#include "Player.h"
#include "Game.h"
#include "NetworkSystem/NetworkProxy.h"

#include <map>
#include <vector>
#include <memory>
#include <functional>

#include <SFML/System/Time.hpp>

template <typename>
struct is_player : std::false_type {};
template <>
struct is_player<CPlayer> : std::true_type {};

/**
 * @class CActorSystem
 * System containing actors. Provides the interface for creating, deleting and processing actors.
 */
class CActorSystem
{
public:

	CActorSystem() = default;
	CActorSystem(const CActorSystem&) = delete;
	
	/**
	 * @function CreateActor
	 * Create an actor with the specified type. Notify the network about it.
	 * 
	 * @template param T - type of the actor to be created.
	 * @template params V - types of the actor creation params.
	 * @params args - params to be passed into the actor's contructor.
	 */
	template<typename T, typename... V>
	SmartId CreateActor(V&&... args)
	{
		std::unique_ptr<CActor> pActor = std::make_unique<T>(std::forward<V>(args)...);
		SmartId sid = pActor->GetEntityId();
		if (sid != InvalidLink)
		{
			m_actors[sid] = std::move(pActor);

			CGame::Get().GetNetworkProxy()->SendCreateActor(sid, m_actors[sid]->GetType(), std::forward<V>(args)...);

			if constexpr (is_player<T>::value)
			{
				static_cast<CPlayer*>(m_actors[sid].get())->SetShooting(m_bPlayersShooting);
			}
		}

		return sid;
	}

	/**
	 * @function CreateActor
	 * Non-generic actors creation. Used by the network to create an actor by the enum type.
	 * 
	 * @template params V - types of the actor creation params.
	 * @param type - type of the actor to be created.
	 * @params args - params to be passed into the actor's contructor.
	 */
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

	/**
	 * @function RemoveActor
	 * Remove actor with the specified entity id. By default performs
	 * the safe deferred deletion.
	 * 
	 * @param sid - SmartId of the actor's entity.
	 * @param immediate - delete the actor immediately or on the end of the frame.
	 */
	void RemoveActor(SmartId sid, bool immediate = false);

	void SetPlayersShooting(bool bShooting);

	CActor* GetActor(SmartId sid);

	/**
	 * @function ForEachPlayer
	 * Iterate over all the players in the system.
	 * 
	 * @param f - function which the actors is applied to.
	 */
	void ForEachPlayer(std::function<bool(CPlayer*)> f);
	int GetNumPlayers() const;
	SmartId GetFirstPlayerId() const;
	SmartId GetLastPlayerId() const;

	void RemoveProjectiles();

	/**
	 * @function Update
	 * Function to update the system's and all the actors' states.
	 * Should be called each frame.
	 * 
	 * @param dt - delta time since the last function call.
	 */
	void Update(sf::Time dt);

	/**
	 * @function CollectGarbage
	 * Delete all the actors in the deferred removal list.
	 */
	void CollectGarbage();

	/**
	 * @function Release
	 * Delete all the actors in the system.
	 */
	void Release();

	/**
	 * @function Serialize
	 * Serialize all the actors' states. Each actor gets into the packet
	 * only if it's NeedSerialize function returns true. But in general
	 * serialization function is called every frame.
	 * 
	 * @param packet - network data packet (received or to send).
	 * @param bReading - determines which serialization mode is needed to use.
	 */
	void Serialize(sf::Packet& packet, bool bReading);

private:
	
	std::map<SmartId, std::unique_ptr<CActor>> m_actors;
	std::vector<SmartId> m_removeDeferred;

	bool m_bPlayersShooting = false;
};