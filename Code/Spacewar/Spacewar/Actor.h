#pragma once

#include <string>

#include <SFML/System/Time.hpp>

#include "LogicalEntity.h"
#include "PhysicalEntity.h"

enum class EActorType
{
	Player,
	Hole,
	Projectile,
	Bonus
};

class CActor : public IPhysicalEventListener
{
public:

	CActor(const std::string& entityName);
	virtual ~CActor();

	CLogicalEntity* GetEntity();

	virtual void OnCollision(SmartId sid) override {}
	virtual EActorType GetType() const = 0;
	virtual void Update(sf::Time dt) = 0;

	void Destroy();

protected:

	SmartId m_entityId = InvalidLink;
};