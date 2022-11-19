#include "LogicalSystem.h"

void CLogicalSystem::Update(sf::Time dt)
{
	for (auto& entity : m_entities)
	{
		entity.first.Update(dt);
	}
}