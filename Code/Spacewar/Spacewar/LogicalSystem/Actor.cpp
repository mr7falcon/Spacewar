#include "StdAfx.h"
#include "Actor.h"
#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "LevelSystem.h"
#include "Game.h"
#include "PhysicalSystem/PhysicalSystem.h"

CActor::CActor(const std::string& entityName)
{
	m_entityId = CGame::Get().GetLogicalSystem()->CreateEntityFromClass(entityName);
	if (m_entityId != InvalidLink)
	{
		if (CPhysicalEntity* pPhysics = CGame::Get().GetPhysicalSystem()->GetEntity(GetEntity()->GetPhysicalEntityId()))
		{
			pPhysics->RegisterEventListener(this);
		}

		if (CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame())
		{
			SetNeedSerialize();
		}
	}
}

CActor::~CActor()
{
	if (m_entityId != InvalidLink)
	{
		if (CPhysicalEntity* pPhysics = CGame::Get().GetPhysicalSystem()->GetEntity(GetEntity()->GetPhysicalEntityId()))
		{
			pPhysics->UnregisterEventListener(this);
		}
		CGame::Get().GetLogicalSystem()->RemoveEntity(m_entityId);
	}
}

CLogicalEntity* CActor::GetEntity()
{
	return CGame::Get().GetLogicalSystem()->GetEntity(m_entityId);
}

void CActor::Destroy()
{
	if (!CGame::Get().GetLogicalSystem()->GetLevelSystem()->IsInGame() || CGame::Get().IsServer())
	{
		CGame::Get().GetLogicalSystem()->GetActorSystem()->RemoveActor(m_entityId);
	}
}

void CActor::SetNeedSerialize()
{
	if (CGame::Get().IsServer())
	{
		m_bNeedSerialize = true;
	}
}

void CActor::Serialize(sf::Packet& packet, uint8_t mode, uint16_t& size)
{
	CLogicalEntity* pEntity = GetEntity();

	sf::Vector2f vPos = pEntity->GetPosition();
	float fRot = pEntity->GetRotation();
	float fScale = pEntity->GetScale();
	sf::Vector2f vVel = pEntity->GetVelocity();
	float fAngSpeed = pEntity->GetAngularSpeed();
	
	SerializeParameters(packet, mode, size, vPos, fRot, fScale, vVel, fAngSpeed);

	pEntity->SetPosition(vPos);
	pEntity->SetRotation(fRot);
	pEntity->SetScale(fScale);
	pEntity->SetVelocity(vVel);
	pEntity->SetAngularSpeed(fAngSpeed);

	m_bNeedSerialize = false;
	m_lastSerialize.restart();
}