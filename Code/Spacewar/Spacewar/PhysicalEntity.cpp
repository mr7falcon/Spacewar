#include "PhysicalEntity.h"

void CPhysicalEntity::OnTransformChanged(const sf::Transform& transform)
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Transform(m_transform.getInverse().combine(transform));
	}
	m_transform = transform;
}