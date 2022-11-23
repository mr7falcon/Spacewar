#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "Game.h"

void CRenderSystem::Render(sf::RenderTarget& target)
{
	ForEachEntity([&](CRenderEntity& entity) { entity.Render(target); }, m_dNumActiveEntities);
}

void CRenderSystem::FixNumActiveEntities()
{
	m_dNumActiveEntities = GetNumEntities();
}

const sf::Texture* CRenderSystem::LoadTexture(int id)
{
	auto fnd = m_textures.find(id);
	if (fnd != m_textures.end())
	{
		return &fnd->second;
	}

	if (const sf::Image* pImage = CGame::Get().GetResourceSystem()->GetTexture(id))
	{
		sf::Texture texture;
		if (texture.loadFromImage(*pImage))
		{
			m_textures[id] = std::move(texture);
			return &m_textures[id];
		}
	}

	return nullptr;
}