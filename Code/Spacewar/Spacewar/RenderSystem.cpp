#include "RenderSystem.h"

void CRenderSystem::Render(sf::RenderTarget& target)
{
	ForEachEntity([&](CRenderEntity& entity) { entity.Render(target); }, m_dNumActiveEntities);
}

void CRenderSystem::FixNumActiveEntities()
{
	m_dNumActiveEntities = GetNumEntities();
}

bool CRenderSystem::IsActiveEntity(SmartId sid) const
{
	if (sid < m_smartLinks.size() && m_smartLinks[sid] < m_dNumActiveEntities)
	{
		return true;
	}
	return false;
}