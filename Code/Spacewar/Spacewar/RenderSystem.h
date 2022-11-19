#pragma once

#include "EntitySystem.h"
#include "RenderEntity.h"

class CRenderSystem : public CEntitySystem<CRenderEntity, true>
{
public:

	CRenderSystem() : CEntitySystem(256) {}

	void Render(sf::RenderTarget& target);
};