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

	sf::Vector2f levelSize = m_pLevelSystem->GetLevelSize();
	auto checkLevelBoundaries = [](float coord, float maxCoord) -> float
	{
		if (coord < 0.f)
			return maxCoord;
		if (coord > maxCoord)
			return 0.f;
		return coord;
	};
	
	ForEachEntity([&](CLogicalEntity& entity) 
		{ 
			entity.Update(dt);

			sf::Vector2f vPos = entity.GetPosition();
			vPos.x = checkLevelBoundaries(vPos.x, levelSize.x);
			vPos.y = checkLevelBoundaries(vPos.y, levelSize.y);
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
				SmartId physicalEntityId = CGame::Get().GetPhysicalSystem()->CreateEntityWithPrimitive(pEntityClass->physicsType, pEntityClass->pPhysics.get());
				pEntity->SetPhysics(physicalEntityId);
			}

			if (pEntityClass->texture >= 0)
			{
				SmartId renderEntityId = CGame::Get().GetRenderSystem()->CreateEntity();
				pEntity->SetRender(renderEntityId);
				CRenderProxy* pRenderProxy = CGame::Get().GetRenderProxy();
				pRenderProxy->OnCommand<CRenderProxy::ERenderCommand_SetTexture>(renderEntityId, pEntityClass->texture);
				pRenderProxy->OnCommand<CRenderProxy::ERenderCommand_SetScale>(renderEntityId, pEntityClass->vScale);
			}
		}

		return sid;
	}
	return InvalidLink;
}