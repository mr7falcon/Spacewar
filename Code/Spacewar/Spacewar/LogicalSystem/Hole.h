#pragma once

#include "Actor.h"

/**
 * @class CHole
 * Hole is an actor placed in the world, that gravitate all the players to itself.
 * Also holes teleportate players, which it collides with, in the random point on the map.
 * This class describes all the holes' logic.
 */
class CHole : public CActor
{
public:

	CHole(const std::string& entity);

	void SetGravityForce(float fGravityForce);

	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType_Hole; }
	virtual void Update(sf::Time dt) override;
	virtual void Serialize(sf::Packet& packet, uint8_t mode, uint16_t& size) override;

private:

	float m_fGravityForce = 0.f;
};