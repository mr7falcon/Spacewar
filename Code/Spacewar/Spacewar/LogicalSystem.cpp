#include "LogicalSystem.h"
#include "ConfigurationSystem.h"
#include "EntityConfiguration.h"
#include "PhysicalSystem.h"
#include "RenderSystem.h"
#include "RenderProxy.h"
#include "Game.h"

void CLogicalSystem::Update(sf::Time dt)
{
	ForEachEntity([dt](CLogicalEntity& entity) { entity.Update(dt); });
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