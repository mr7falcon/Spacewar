#pragma once

#include <memory>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "EntitySystem.h"

class CRenderEntity : public CEntity
{
public:

	CRenderEntity() = default;

	void SetTexture(const sf::Texture* pTexture);
	void SetTransform(const sf::Transform& transform) { m_transform = transform; }
	void SetScale(const sf::Vector2f& scale) { m_sprite.setScale(scale); }

	void Render(sf::RenderTarget& target) const;

private:

	sf::Sprite m_sprite;
	sf::Transform m_transform;
};