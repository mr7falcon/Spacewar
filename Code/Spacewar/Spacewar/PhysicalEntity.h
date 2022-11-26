#pragma once

#include <memory>
#include <vector>

#include "EntitySystem.h"
#include "PhysicalPrimitive.h"

class IPhysicalEventListener
{
public:

	virtual void OnCollision(SmartId sid) = 0;
};

class CPhysicalEntity : public CEntity
{
public:

	CPhysicalEntity() = default;
	CPhysicalEntity(std::unique_ptr<PhysicalPrimitive::IPrimitive> pPrimitive)
		: m_pPrimitive(std::move(pPrimitive)) {}
	
	const PhysicalPrimitive::IPrimitive* GetPhysics() const { return m_pPrimitive.get(); }

	void OnTransformChanged(const sf::Transform& transform);
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