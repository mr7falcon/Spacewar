#include "LogicalSystem.h"

void CLogicalSystem::Update(sf::Time dt)
{
	ForEachEntity([dt](CLogicalEntity& entity) { entity.Update(dt); });
}