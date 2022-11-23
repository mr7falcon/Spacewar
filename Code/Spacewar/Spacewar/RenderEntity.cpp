#include "RenderEntity.h"
#include "Game.h"

void CRenderEntity::SetTexture(const sf::Texture* pTexture)
{
	m_sprite.setTexture(*pTexture);
	sf::FloatRect bounds = m_sprite.getLocalBounds();
	m_sprite.setOrigin(0.5f * bounds.width, 0.5f * bounds.height);
	m_sprite.setScale(sf::Vector2f(0.2f, 0.2f));
}

void CRenderEntity::Render(sf::RenderTarget& target) const
{
	sf::RenderStates states;
	states.transform = m_transform;
	target.draw(m_sprite, states);
}