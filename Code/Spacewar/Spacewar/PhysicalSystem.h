#pragma once

#include <memory>

#include "PhysicalEntity.h"
#include "EntityConfiguration.h"

class CPhysicalSystem : public CEntitySystem <CPhysicalEntity, false>
{
public:

	CPhysicalSystem() : CEntitySystem(128) {}

	SmartId CreateEntityWithPrimitive(PhysicalPrimitive::EPrimitiveType type, const CEntityConfiguration::IPrimitiveConfig* pConfig)
	{
		std::unique_ptr<PhysicalPrimitive::Primitive> pPrimitive;

		switch (type)
		{
		case PhysicalPrimitive::EPrimitiveType_Circle:
			if (const CEntityConfiguration::CircleConfig* pCircleConfig = static_cast<const CEntityConfiguration::CircleConfig*>(pConfig))
			{
				pPrimitive = std::make_unique<PhysicalPrimitive::Circle>(pCircleConfig->fRad);
			}
			break;
		case PhysicalPrimitive::EPrimitiveType_Capsule:
			if (const CEntityConfiguration::CapsuleConfig* pCapsuleConfig = static_cast<const CEntityConfiguration::CapsuleConfig*>(pConfig))
			{
				pPrimitive = std::make_unique<PhysicalPrimitive::Capsule>(pCapsuleConfig->vAxis, pCapsuleConfig->fHalfHeight, pCapsuleConfig->fRad);
			}
			break;
		}

		return CreateEntity(std::move(pPrimitive));
	}

	void ProcessCollisions();
};