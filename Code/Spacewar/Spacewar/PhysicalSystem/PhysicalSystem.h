#pragma once

#include "PhysicalEntity.h"
#include "ConfigurationSystem/EntityConfiguration.h"

class CPhysicalSystem : public CEntitySystem <CPhysicalEntity, false>
{
public:

	CPhysicalSystem() : CEntitySystem(128) {}

	SmartId CreateEntityWithPrimitive(PhysicalPrimitive::EPrimitiveType type, const CEntityConfiguration::IPrimitiveConfig* pConfig);

	void ProcessCollisions();
};