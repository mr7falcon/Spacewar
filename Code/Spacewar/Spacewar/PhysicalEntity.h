#pragma once

#include <memory>

#include "EntitySystem.h"
#include "PhysicalPrimitive.h"

class CPhysicalEntity
{
public:

	CPhysicalEntity() = default;

private:

	SmartId m_logicalEntityId = InvalidLink;
	std::unique_ptr<IPhysicalPrimitive> m_pPrimitive;

};