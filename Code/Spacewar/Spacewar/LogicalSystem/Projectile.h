#pragma once

#include "Actor.h"

/**
 * @class CProjectile
 * Projectile is an actor spawned by the players and moving in the specified direction.
 * Colliding with other players, projectiles are destroyed 
 * along with the player which are they collided with.
 * This class describes all the projectiles' logic.
 */
class CProjectile : public CActor
{
public:

	CProjectile(const std::string& entity);

	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType_Projectile; }
	virtual void Update(sf::Time dt) override;

	void SetLifetime(float fLifetime);
	void SetOwnerId(SmartId sid);
	SmartId GetOwnerId() const { return m_owner; }

private:

	float m_fLifetime = 0.f;
	SmartId m_owner = InvalidLink;
};