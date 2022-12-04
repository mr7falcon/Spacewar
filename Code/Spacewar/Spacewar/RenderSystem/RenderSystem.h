#pragma once

#include "EntitySystem.h"
#include "RenderEntity.h"

#include <unordered_map>

#include <SFML/Graphics/Texture.hpp>

class CRenderSystem : public CEntitySystem<CRenderEntity, true>
{
public:

	CRenderSystem() : CEntitySystem(256) {}

	void FixNumActiveEntities();
	
	const sf::Texture* LoadTexture(int id);

	void Render(sf::RenderTarget& target);

private:

	int m_dNumActiveEntities = 0;

	std::unordered_map<int, sf::Texture> m_textures;
};