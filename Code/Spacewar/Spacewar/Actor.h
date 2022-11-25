#pragma once

#include <string>

#include <SFML/System/Time.hpp>

#include "LogicalEntity.h"

class CActor
{
public:

	CActor(const std::string& entityName);

	CLogicalEntity* GetEntity();

	virtual bool IsPlayer() const { return false; }
	virtual void Update(sf::Time dt) = 0;

private:

	SmartId m_entityId = InvalidLink;
};