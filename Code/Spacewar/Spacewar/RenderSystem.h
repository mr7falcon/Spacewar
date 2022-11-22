#pragma once

#include "EntitySystem.h"
#include "RenderEntity.h"

class CRenderSystem : public CEntitySystem<CRenderEntity, true>
{
public:

	CRenderSystem() : CEntitySystem(256) {}

	void FixNumActiveEntities();
	bool IsActiveEntity(SmartId sid) const;

	void Render(sf::RenderTarget& target);

private:

	uint32_t m_dNumActiveEntities = 0;
};