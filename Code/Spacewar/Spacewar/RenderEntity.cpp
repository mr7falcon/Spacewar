#include "RenderEntity.h"

CRenderEntity::CRenderEntity(ERenderObjectType type)
{
	m_type = type;
	switch (m_type)
	{
	case Sprite:
		m_pRenderObject = std::make_unique<sf::Sprite>();
		break;
	case Text:
		m_pRenderObject = std::make_unique<sf::Text>();
		break;
	}
}

void CRenderEntity::RecoverOrigin()
{
	switch (m_type)
	{
	case Sprite:
		if (auto* pSprite = static_cast<sf::Sprite*>(m_pRenderObject.get()))
		{
			sf::FloatRect bounds = pSprite->getLocalBounds();
			pSprite->setOrigin(0.5f * bounds.width, 0.5f * bounds.height);
		}
		break;
	case Text:
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			sf::FloatRect bounds = pText->getLocalBounds();
			pText->setOrigin(0.5f * bounds.width, 0.5f * bounds.height);
		}
	}
}

void CRenderEntity::SetTexture(const sf::Texture* pTexture)
{
	if (m_type == Sprite)
	{
		if (auto* pSprite = static_cast<sf::Sprite*>(m_pRenderObject.get()))
		{
			pSprite->setTexture(*pTexture);
			RecoverOrigin();
		}
	}
}

const sf::Texture* CRenderEntity::GetTexture() const
{
	if (m_type == Sprite)
	{
		if (auto* pSprite = static_cast<sf::Sprite*>(m_pRenderObject.get()))
		{
			return pSprite->getTexture();
		}
	}
	return nullptr;
}

void CRenderEntity::SetSize(const sf::Vector2f& vSize)
{
	if (m_type == Sprite)
	{
		if (auto* pSprite = static_cast<sf::Sprite*>(m_pRenderObject.get()))
		{
			const sf::FloatRect& bbox = pSprite->getLocalBounds();
			pSprite->setScale(sf::Vector2f(bbox.width > 0.f ? vSize.x / bbox.width : 0.f, bbox.height > 0.f ? vSize.y / bbox.height : 0.f));
		}
	}
}

void CRenderEntity::SetText(const std::string& text)
{
	if (m_type == Text)		// make this assert ?
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			pText->setString(text);
			RecoverOrigin();
		}
	}
}

const std::string& CRenderEntity::GetText() const
{
	if (m_type == Text)
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			return pText->getString();
		}
	}
	static constexpr const char* ReturnDefault = "";
	return ReturnDefault;
}

void CRenderEntity::SetColor(const sf::Color color)
{
	switch (m_type)
	{
	case Sprite:
		if (auto* pSprite = static_cast<sf::Sprite*>(m_pRenderObject.get()))
		{
			pSprite->setColor(color);
		}
		break;
	case Text:
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			pText->setFillColor(color);
		}
		break;
	}
}

sf::Color CRenderEntity::GetColor() const
{
	switch (m_type)
	{
	case Sprite:
		if (auto* pSprite = static_cast<sf::Sprite*>(m_pRenderObject.get()))
		{
			return pSprite->getColor();
		}
		break;
	case Text:
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			return pText->getFillColor();
		}
		break;
	}
	return sf::Color::Black;
}

void CRenderEntity::SetCharacterSize(unsigned int size)
{
	if (m_type == Text)
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			pText->setCharacterSize(size);
			RecoverOrigin();
		}
	}
}

unsigned int CRenderEntity::GetCharacterSize() const
{
	if (m_type == Text)
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			return pText->getCharacterSize();
		}
	}
	return 0;
}

void CRenderEntity::SetFont(const sf::Font& font)
{
	if (m_type == Text)
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			pText->setFont(font);
			RecoverOrigin();
		}
	}
}

const sf::Font* CRenderEntity::GetFont() const
{
	if (m_type == Text)
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			return pText->getFont();
		}
	}
	return nullptr;
}

void CRenderEntity::SetStyle(uint32_t style)
{
	if (m_type == Text)
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			pText->setStyle(style);
			RecoverOrigin();
		}
	}
}

uint32_t CRenderEntity::GetStyle() const
{
	if (m_type == Text)
	{
		if (auto* pText = static_cast<sf::Text*>(m_pRenderObject.get()))
		{
			return pText->getStyle();
		}
	}
	return sf::Text::Regular;
}

const sf::Vector2f CRenderEntity::GetSize() const
{
	sf::FloatRect rect;
	switch (m_type)
	{
	case Sprite:
		rect = static_cast<sf::Sprite*>(m_pRenderObject.get())->getLocalBounds();
		break;
	case Text:
		rect = static_cast<sf::Text*>(m_pRenderObject.get())->getLocalBounds();
		break;
	}
	return sf::Vector2f(rect.width, rect.height);
}

void CRenderEntity::Render(sf::RenderTarget& target) const
{
	sf::RenderStates states;
	states.transform = m_transform;
	target.draw(*m_pRenderObject, states);
}