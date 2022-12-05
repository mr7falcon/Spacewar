#pragma once

#include "EntitySystem.h"
#include "PhysicalPrimitive.h"

#include <memory>
#include <vector>

/**
 * @interface IPhysicalEventListener
 * Interface for the collisions' processing.
 * Each IPhysicalEventListener must be registered in the corresponding
 * CPhysicalEntity instance and unregistered before the pointer invalidation.
 */
class IPhysicalEventListener
{
public:

	virtual void OnCollision(SmartId sid) = 0;
};

/**
 * @class CPhysicalEntity
 * Physical entity presents the geometrical representation of the game objects.
 * It owns one of the geometrical primitives, which provide the collision detecion functions.
 * Physical entity is mainly created by the logical entity, so the collision events
 * are sent in the logical system in respect with them.
 */
class CPhysicalEntity : public CEntity
{
public:

	CPhysicalEntity() = default;
	CPhysicalEntity(std::unique_ptr<PhysicalPrimitive::IPrimitive> pPrimitive)
		: m_pPrimitive(std::move(pPrimitive)) {}
	
	const PhysicalPrimitive::IPrimitive* GetPhysics() const { return m_pPrimitive.get(); }

	/**
	 * @function OnTransformChanged
	 * Recalculate the geometrical properties of the entity.
	 * 
	 * @param transform - the new entity transform.
	 */
	void OnTransformChanged(const sf::Transform& transform);

	/**
	 * @function OnCollision
	 * Retranslate the collision event to the event listeners.
	 * 
	 * @param sid - SmartId of the logical entity which the object collided with.
	 */
	void OnCollision(SmartId sid);

	void RegisterEventListener(IPhysicalEventListener* pListener);
	void UnregisterEventListener(IPhysicalEventListener* pListener);

	void SetParentEntityId(SmartId sid) { m_parentEntityId = sid; }
	SmartId GetParentEntityId() const { return m_parentEntityId; }

private:
	
	std::unique_ptr<PhysicalPrimitive::IPrimitive> m_pPrimitive;
	sf::Transform m_transform;

	std::vector<IPhysicalEventListener*> m_eventListeners;
	SmartId m_parentEntityId = InvalidLink;
};