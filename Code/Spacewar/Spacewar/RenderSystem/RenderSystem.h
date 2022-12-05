#pragma once

#include "EntitySystem.h"
#include "RenderEntity.h"

#include <unordered_map>

#include <SFML/Graphics/Texture.hpp>

/**
 * @class CRenderSystem
 * It is a simple system to store and process the render entities.
 * The main purpose of the render system is to draw some staff in the game window.
 * Since the render system works in a separated thread, it uses the safe
 * CEntitySystem instance. The number of the entities to process at the end 
 * of each frame is fixed and cannot be modifier until the next synchronization.
 * Besides it, main thread cannot "physically" delete entities from the system.
 * It can just unlink them so they will be deleted in the end of the frame.
 * The render system also stores the textures instances, created in the video memory.
 */
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