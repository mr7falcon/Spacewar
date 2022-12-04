#include "StdAfx.h"
#include "LogicalSystem.h"
#include "Game.h"
#include "ActorSystem.h"
#include "LevelSystem.h"
#include "Player.h"
#include "FeedbackSystem.h"
#include "ConfigurationSystem/ConfigurationSystem.h"
#include "ConfigurationSystem/EntityConfiguration.h"
#include "PhysicalSystem/PhysicalSystem.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/RenderProxy.h"

CLogicalSystem::CLogicalSystem()
	: CEntitySystem(64)
	, m_pActorSystem(std::make_unique<CActorSystem>())
	, m_pLevelSystem(std::make_unique<CLevelSystem>())
	, m_pFeedbackSystem(std::make_unique<CFeedbackSystem>())
{}

CLogicalSystem::~CLogicalSystem() = default;

void CLogicalSystem::Release()
{
	m_pActorSystem.reset();
	m_pLevelSystem.reset();
	m_pFeedbackSystem.reset();
}

void CLogicalSystem::Update(sf::Time dt)
{
	m_pActorSystem->Update(dt);
	m_pLevelSystem->Update(dt);

	float levelSize = m_pLevelSystem->GetLevelSize();
	auto checkLevelBoundaries = [levelSize](float coord) -> float
	{
		if (coord < 0.f)
			return levelSize + coord;
		if (coord > levelSize)
			return coord - levelSize;
		return coord;
	};
	
	ForEachEntity([&](CLogicalEntity& entity) 
		{ 
			entity.Update(dt);

			sf::Vector2f vPos = entity.GetPosition();
			vPos.x = checkLevelBoundaries(vPos.x);
			vPos.y = checkLevelBoundaries(vPos.y);
			entity.SetPosition(vPos);
		});
}

SmartId CLogicalSystem::CreateEntityFromClass(const std::string& name)
{
	if (const CEntityConfiguration::SEntityClass* pEntityClass = CGame::Get().GetConfigurationSystem()->GetEntityConfiguration()->GetEntityClass(name))
	{
		SmartId sid = CreateEntity();

		if (CLogicalEntity* pEntity = GetEntity(sid))
		{
			if (pEntityClass->physicsType != PhysicalPrimitive::EPrimitiveType_Num)
			{
				CPhysicalSystem* pPhysicalSystem = CGame::Get().GetPhysicalSystem();
				SmartId physicalEntityId = pPhysicalSystem->CreateEntityWithPrimitive(pEntityClass->physicsType, pEntityClass->pPhysics.get());
				pEntity->SetPhysics(physicalEntityId);
				if (CPhysicalEntity* pPhysics = pPhysicalSystem->GetEntity(physicalEntityId))
				{
					pPhysics->SetParentEntityId(sid);
				}
			}

			SmartId renderEntityId = CGame::Get().GetRenderSystem()->CreateEntity(CRenderEntity::Sprite);
			pEntity->SetRender(renderEntityId);

			for (const auto& slot : pEntityClass->renderSlots)
			{
				pEntity->AddRenderSlot(slot);
			}
			pEntity->ActivateRenderSlot(0);
		}

		return sid;
	}
	else
	{
		Log("Failed to find entity class ", name);
	}
	return InvalidLink;
}

void CLogicalSystem::RemoveEntity(SmartId sid, bool immediate)
{
	if (CLogicalEntity* pEntity = GetEntity(sid))
	{
		SmartId physicalEntityId = pEntity->GetPhysicalEntityId();
		if (physicalEntityId != InvalidLink)
		{
			CGame::Get().GetPhysicalSystem()->RemoveEntity(physicalEntityId, immediate);
		}

		SmartId renderEntityId = pEntity->GetRenderEntityId();
		if (renderEntityId != InvalidLink)
		{
			CGame::Get().GetRenderSystem()->RemoveEntity(renderEntityId, immediate);
		}

		CEntitySystem::RemoveEntity(sid, immediate);
	}
}

void CLogicalSystem::CollectGarbage()
{
	m_pActorSystem->CollectGarbage();
	CEntitySystem::CollectGarbage();
}

void CLogicalSystem::Clear()
{
	m_pActorSystem->Release();
	CEntitySystem::Clear();
}