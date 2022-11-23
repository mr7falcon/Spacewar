#include "PhysicalSystem.h"
#include "Game.h"
#include "LogicalSystem.h"

void CPhysicalSystem::ProcessCollisions()
{
	for (int i = 0; i < m_entities.size(); ++i)
	{
		for (int j = i + 1; j < m_entities.size(); ++j)
		{
			const auto pPhysics1 = m_entities[i].first.GetPhysics();
			const auto pPhysics2 = m_entities[j].first.GetPhysics();

			if (g_intersectionsTable[pPhysics1->GetType()][pPhysics2->GetType()](pPhysics1, pPhysics2))
			{
				// OnCollision
			}
		}
	}
}