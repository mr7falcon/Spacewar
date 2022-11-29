#include "LogicalSystem.h"
#include "ActorSystem.h"
#include "LevelSystem.h"
#include "ConfigurationSystem.h"
#include "EntityConfiguration.h"
#include "PhysicalSystem.h"
#include "RenderSystem.h"
#include "RenderProxy.h"
#include "Game.h"
#include "Player.h"

CLogicalSystem::CLogicalSystem()
	: CEntitySystem(64)
	, m_pActorSystem(std::make_unique<CActorSystem>())
	, m_pLevelSystem(std::make_unique<CLevelSystem>())
{}

CLogicalSystem::~CLogicalSystem() = default;

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

			if (pEntityClass->texture >= 0)
			{
				SmartId renderEntityId = CGame::Get().GetRenderSystem()->CreateEntity(CRenderEntity::Sprite);
				pEntity->SetRender(renderEntityId);
				CRenderProxy* pRenderProxy = CGame::Get().GetRenderProxy();
				pRenderProxy->OnCommand<CRenderProxy::ERenderCommand_SetTexture>(renderEntityId, pEntityClass->texture);
				if (pEntityClass->vSize != sf::Vector2f())
				{
					pRenderProxy->OnCommand<CRenderProxy::ERenderCommand_SetSize>(renderEntityId, pEntityClass->vSize);
				}
			}
		}

		return sid;
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