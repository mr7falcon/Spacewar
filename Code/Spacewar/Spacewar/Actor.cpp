#include "Actor.h"
#include "LogicalSystem.h"
#include "PhysicalSystem.h"
#include "ActorSystem.h"
#include "Game.h"

CActor::CActor(const std::string& entityName)
{
	m_entityId = CGame::Get().GetLogicalSystem()->CreateEntityFromClass(entityName);
	if (CPhysicalEntity* pPhysics = CGame::Get().GetPhysicalSystem()->GetEntity(GetEntity()->GetPhysicalEntityId()))
	{
		pPhysics->RegisterEventListener(this);
	}
}

CActor::~CActor()
{
	if (CPhysicalEntity* pPhysics = CGame::Get().GetPhysicalSystem()->GetEntity(GetEntity()->GetPhysicalEntityId()))
	{
		pPhysics->UnregisterEventListener(this);
	}
	CGame::Get().GetLogicalSystem()->RemoveEntity(GetEntity()->GetId());
}

CLogicalEntity* CActor::GetEntity()
{
	return CGame::Get().GetLogicalSystem()->GetEntity(m_entityId);
}

void CActor::Destroy()
{
	CGame::Get().GetLogicalSystem()->GetActorSystem()->RemoveActor(m_entityId);
}