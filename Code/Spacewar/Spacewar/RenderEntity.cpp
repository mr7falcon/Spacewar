#include "RenderEntity.h"

void CRenderEntity::Render(sf::RenderTarget& target) const
{
	if (!m_pObject)
		return;

	auto pTexture = m_pTexture.lock();

	sf::RenderStates states;
	states.transform = m_transform;
	states.texture = pTexture.get();

	target.draw(*m_pObject, states);		// is the texture required after this call?
}