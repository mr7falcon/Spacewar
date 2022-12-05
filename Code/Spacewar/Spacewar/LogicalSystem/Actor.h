#pragma once

#include "LogicalEntity.h"
#include "PhysicalSystem/PhysicalEntity.h"

#include <string>

#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Network/Packet.hpp>

enum EActorType : uint8_t
{
	EActorType_Player,
	EActorType_Hole,
	EActorType_Projectile,
	EActorType_Bonus
};

/**
 * @class CActor
 * Base class for the all ingame actors. Each actor has an owner
 * logical entity. In most functions this entity id also is an actor id.
 */
class CActor : public IPhysicalEventListener
{
public:

	CActor(const std::string& entityName);
	virtual ~CActor();

	CLogicalEntity* GetEntity();
	SmartId GetEntityId() const { return m_entityId; }

	/**
	 * @function OnCollision
	 * Inherited from the IPhysicalEventListener function to handle the entities collision.
	 * 
	 * @param sid = SmartId of the logical entity id, which the actor collided with.
	 */
	virtual void OnCollision(SmartId sid) override {}

	virtual EActorType GetType() const = 0;
	virtual void Update(sf::Time dt) = 0;

	/**
	 * @function NeedSerialize
	 * Function to check if the actor needs to be serialized. Each game actor should call this
	 * function if it's network state is changed. Otherwise the actors are mark serializable each second.
	 * 
	 * @return True if the actor needs to be serialized, false otherwise.
	 */
	bool NeedSerialize() const { return m_bNeedSerialize || m_lastSerialize.getElapsedTime().asSeconds() > 1.f; }

	/**
	 * @function SetNeedSerialize
	 * Mark actor as serializable so that it gets into the nearest packet.
	 */
	void SetNeedSerialize();

	/**
	 * @function Serialize
	 * Serialize the current actor's state to replicate it on the client side.
	 * The default CActor's Serialie method cares about the owner logical entity state.
	 * All the inherits can override it to replicate their own date but they also
	 * must to call the base function to serialize the logical entity too.
	 * For the additional information see CActorSystem and CNetworkProxy.
	 * 
	 * @param packet - network data packet (received or to send).
	 * @param mode - serialization mode (read, write, count).
	 * @param size - output size of the actor serialization structure.
	 */
	virtual void Serialize(sf::Packet& packet, uint8_t mode, uint16_t& size);

	/**
	 * @function Destroy
	 * Mark the actor as destroyed. Destroyed actors are removed from the actors system then.
	 * @note Only server can actually destroy actors using this method.
	 */
	void Destroy();

protected:

	SmartId m_entityId = InvalidLink;

	bool m_bNeedSerialize = false;
	sf::Clock m_lastSerialize;
};