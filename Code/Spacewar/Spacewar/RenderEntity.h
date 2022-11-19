#pragma once

#include <memory>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

class CRenderEntity
{
public:

	CRenderEntity() = default;
	CRenderEntity(std::unique_ptr<sf::Drawable>&& pObject)
		: m_pObject(std::forward<std::unique_ptr<sf::Drawable>>(pObject)) {}

	void SetTexture(const std::weak_ptr<sf::Texture>& pTexture) { m_pTexture = pTexture; }
	void SetTransform(const sf::Transform& transform) { m_transform = transform; }

	sf::Drawable* GetRenderObject() const { return m_pObject.get(); }

	void Render(sf::RenderTarget& target) const;

private:

	std::unique_ptr<sf::Drawable> m_pObject;

	std::weak_ptr<sf::Texture> m_pTexture;
	sf::Transform m_transform;
};