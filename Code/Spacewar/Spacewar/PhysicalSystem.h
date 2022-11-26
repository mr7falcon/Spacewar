#pragma once

#include <memory>

#include "PhysicalEntity.h"
#include "EntityConfiguration.h"

class CPhysicalSystem : public CEntitySystem <CPhysicalEntity, false>
{
public:

	CPhysicalSystem() : CEntitySystem(128) {}

	SmartId CreateEntityWithPrimitive(PhysicalPrimitive::EPrimitiveType type, const CEntityConfiguration::IPrimitiveConfig* pConfig);

	void ProcessCollisions();
};