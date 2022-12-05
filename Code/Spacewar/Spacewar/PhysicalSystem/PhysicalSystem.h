#pragma once

#include "PhysicalEntity.h"
#include "ConfigurationSystem/EntityConfiguration.h"

/**
 * @class CPhysicalSystem
 * That system simply contains all the physical entities
 * and computes collisions between them.
 */
class CPhysicalSystem : public CEntitySystem <CPhysicalEntity, false>
{
public:

	CPhysicalSystem() : CEntitySystem(128) {}

	/**
	 * @function CreateEntityWithPrimitive
	 * Create the physical primitive from the specified configuration
	 * and assign it to the new created physical entity.
	 * 
	 * @param type - physical primitive type code.
	 * @param pConfig - physical primitive configuration.
	 * @return SmartId of the created entity.
	 */
	SmartId CreateEntityWithPrimitive(PhysicalPrimitive::EPrimitiveType type, const CEntityConfiguration::IPrimitiveConfig* pConfig);

	void ProcessCollisions();
};