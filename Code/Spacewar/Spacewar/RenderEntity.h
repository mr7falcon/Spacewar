#pragma once

#include <memory>

#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "EntitySystem.h"

class CRenderEntity : public CEntity
{
public:

	enum ERenderObjectType : uint8_t
	{
		Sprite,
		Text
	};

	CRenderEntity() = default;
	CRenderEntity(ERenderObjectType type);

	
	// Sprite
	void SetTexture(const sf::Texture* pTexture);
	void SetSize(const sf::Vector2f& vSize);

	const sf::Texture* GetTexture() const;
	
	// Text
	void SetText(const std::string& text);
	void SetCharacterSize(unsigned int dSize);
	void SetStyle(uint32_t style);
	void SetFont(const sf::Font& font);

	const std::string& GetText() const;
	unsigned int GetCharacterSize() const;
	uint32_t GetStyle() const;
	const sf::Font* GetFont() const;
	
	// Shared
	void SetTransform(const sf::Transform& transform) { m_transform = transform; }
	void SetColor(const sf::Color color);

	const sf::Transform& GetTransform() const { return m_transform; }
	sf::Color GetColor() const;
	const sf::Vector2f GetSize() const;

	void Render(sf::RenderTarget& target) const;

private:

	void RecoverOrigin();

private:

	std::unique_ptr<sf::Drawable> m_pRenderObject;
	ERenderObjectType m_type;
	sf::Transform m_transform;
};