#pragma once

#include "PhysicalEntity.h"

class CPhysicalSystem : public CEntitySystem <CPhysicalEntity, false>
{
public:

	CPhysicalSystem() : CEntitySystem(128) {}

	void ProcessCollisions();
};