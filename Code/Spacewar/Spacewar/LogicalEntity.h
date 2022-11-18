#pragma once

#include <SFML/Graphics/Transformable.hpp>

#include "EntitySystem.h"

class CLogicalEntity : private sf::Transformable
{
public:

	CLogicalEntity() = default;

private:

	SmartId m_physicalEntityId = InvalidLink;
	SmartId m_renderEntityId = InvalidLink;

};