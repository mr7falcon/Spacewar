#include "Actor.h"
#include "LogicalSystem.h"
#include "Game.h"

CActor::CActor(const std::string& entityName)
{
	m_entityId = CGame::Get().GetLogicalSystem()->CreateEntityFromClass(entityName);
}

CLogicalEntity* CActor::GetEntity()
{
	return CGame::Get().GetLogicalSystem()->GetEntity(m_entityId);
}