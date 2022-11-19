#include "RenderSystem.h"

void CRenderSystem::Render(sf::RenderTarget& target)
{
	for (auto& entity : m_entities)
	{
		entity.first.Render(target);
	}
}