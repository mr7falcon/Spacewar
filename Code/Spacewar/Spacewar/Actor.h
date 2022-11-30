#pragma once

#include <string>

#include <SFML/System/Time.hpp>

#include "LogicalEntity.h"
#include "PhysicalEntity.h"
#include "NetworkSystem.h"

enum EActorType : uint8_t
{
	EActorType_Player,
	EActorType_Hole,
	EActorType_Projectile,
	EActorType_Bonus
};

class CActor : public IPhysicalEventListener
{
public:

	CActor(const std::string& entityName);
	virtual ~CActor();

	CLogicalEntity* GetEntity();
	SmartId GetEntityId() const { return m_entityId; }

	virtual void OnCollision(SmartId sid) override {}
	virtual EActorType GetType() const = 0;
	virtual void Update(sf::Time dt) = 0;

	bool NeedSerialize() const { return m_bNeedSerialize; }
	void SetNeedSerialize();
	virtual void Serialize(sf::Packet& packet, bool bReading);

	void Destroy();

protected:

	SmartId m_entityId = InvalidLink;

	bool m_bNeedSerialize = false;
};