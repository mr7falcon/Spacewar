#pragma once

#include "EntitySystem.h"
#include "RenderEntity.h"

class CRenderSystem : public CEntitySystem<CRenderEntity, true>
{
public:

	CRenderSystem() : CEntitySystem(256) {}

	template <typename T, typename... V>
	SmartId CreateEntity(V... args)
	{
		return CreateEntity<std::unique_ptr<T>>(std::make_unique<T>(args...));
	}

	void Render(sf::RenderTarget& target);
};