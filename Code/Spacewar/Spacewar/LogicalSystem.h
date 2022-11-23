#pragma once

#include <string>

#include "LogicalEntity.h"

class CLogicalSystem : public CEntitySystem<CLogicalEntity, false>
{
public:

	CLogicalSystem() : CEntitySystem(64) {}

	SmartId CreateEntityFromClass(const std::string& name);

	void Update(sf::Time dt);
};