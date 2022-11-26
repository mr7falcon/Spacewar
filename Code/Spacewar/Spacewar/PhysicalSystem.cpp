#include "PhysicalSystem.h"
#include "Game.h"
#include "LogicalSystem.h"

SmartId CPhysicalSystem::CreateEntityWithPrimitive(PhysicalPrimitive::EPrimitiveType type, const CEntityConfiguration::IPrimitiveConfig* pConfig)
{
	std::unique_ptr<PhysicalPrimitive::IPrimitive> pPrimitive;

	switch (type)
	{
	case PhysicalPrimitive::EPrimitiveType_Circle:
		if (const CEntityConfiguration::CircleConfig* pCircleConfig = static_cast<const CEntityConfiguration::CircleConfig*>(pConfig))
		{
			pPrimitive = std::make_unique<PhysicalPrimitive::Circle>(pCircleConfig->fRad);
		}
		break;
	case PhysicalPrimitive::EPrimitiveType_Capsule:
		if (const CEntityConfiguration::CapsuleConfig* pCapsuleConfig = static_cast<const CEntityConfiguration::CapsuleConfig*>(pConfig))
		{
			pPrimitive = std::make_unique<PhysicalPrimitive::Capsule>
				(pCapsuleConfig->fHalfHeight * pCapsuleConfig->vAxis, -pCapsuleConfig->fHalfHeight * pCapsuleConfig->vAxis, pCapsuleConfig->fRad);
		}
		break;
	case PhysicalPrimitive::EPrimitiveType_Polygon:
		if (const CEntityConfiguration::PolygonConfig* pPolygonConfig = static_cast<const CEntityConfiguration::PolygonConfig*>(pConfig))
		{
			pPrimitive = std::make_unique<PhysicalPrimitive::Polygon>(pPolygonConfig->vertices);
		}
		break;
	}

	return CreateEntity(std::move(pPrimitive));
}

void CPhysicalSystem::ProcessCollisions()
{
	for (int i = 0; i < m_entities.size(); ++i)
	{
		for (int j = i + 1; j < m_entities.size(); ++j)
		{
			const auto* pPhysics1 = m_entities[i].GetPhysics();
			const auto* pPhysics2 = m_entities[j].GetPhysics();

			if (g_intersectionsTable[pPhysics1->GetType()][pPhysics2->GetType()](pPhysics1, pPhysics2))
			{
				m_entities[i].OnCollision(m_entities[j].GetParentEntityId());
				m_entities[j].OnCollision(m_entities[i].GetParentEntityId());
			}
		}
	}
}