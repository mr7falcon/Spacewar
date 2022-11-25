#pragma once

#include <memory>
#include "EntitySystem.h"
#include "PhysicalPrimitive.h"

class CPhysicalEntity : public CEntity
{
public:

	CPhysicalEntity() = default;
	CPhysicalEntity(std::unique_ptr<PhysicalPrimitive::Primitive> pPrimitive)
		: m_pPrimitive(std::move(pPrimitive)) {}
	
	const PhysicalPrimitive::Primitive* GetPhysics() const { return m_pPrimitive.get(); }

	void OnTransformChanged(const sf::Transform& transform);

private:
	
	std::unique_ptr<PhysicalPrimitive::Primitive> m_pPrimitive;
	sf::Transform m_transform;
};