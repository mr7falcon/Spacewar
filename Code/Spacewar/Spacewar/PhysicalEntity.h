#pragma once

#include <memory>
#include "EntitySystem.h"
#include "PhysicalPrimitive.h"

class CPhysicalEntity
{
public:

	CPhysicalEntity() = default;
	CPhysicalEntity(std::unique_ptr<PhysicalPrimitive::IPhysicalPrimitive>&& pPrimitive)
		: m_pPrimitive(std::forward<std::unique_ptr<PhysicalPrimitive::IPhysicalPrimitive>>(pPrimitive)) {}
	
	const PhysicalPrimitive::IPhysicalPrimitive* GetPhysics() const { return m_pPrimitive.get(); }

	void OnTransformChanged(const sf::Transform& transform);

private:
	
	std::unique_ptr<PhysicalPrimitive::IPhysicalPrimitive> m_pPrimitive;
	sf::Transform m_transform;
};