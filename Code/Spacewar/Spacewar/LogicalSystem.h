#pragma once

#include "LogicalEntity.h"

class CLogicalSystem : public CEntitySystem<CLogicalEntity, false>
{
public:

	CLogicalSystem() : CEntitySystem(64) {}
};