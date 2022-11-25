#include "RenderEntity.h"
#include "Game.h"

void CRenderEntity::SetTexture(const sf::Texture* pTexture)
{
	m_sprite.setTexture(*pTexture);
	sf::FloatRect bounds = m_sprite.getLocalBounds();
	m_sprite.setOrigin(0.5f * bounds.width, 0.5f * bounds.height);
}

void CRenderEntity::SetSize(const sf::Vector2f& vSize)
{
	const sf::FloatRect& bbox = m_sprite.getLocalBounds();
	m_sprite.setScale(sf::Vector2f(bbox.width > 0.f ? vSize.x / bbox.width : 0.f, bbox.height > 0.f ? vSize.y / bbox.height : 0.f));
}

void CRenderEntity::Render(sf::RenderTarget& target) const
{
	sf::RenderStates states;
	states.transform = m_transform;
	target.draw(m_sprite, states);
}