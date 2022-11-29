#pragma once

#include "Actor.h"

class CProjectile : public CActor
{
public:

	CProjectile();

	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType::Projectile; }
	virtual void Update(sf::Time dt) override;

	void SetLifetime(float fLifetime) { m_fLifetime = fLifetime; }
	void SetOwnerId(SmartId sid) { m_owner = sid; }
	SmartId GetOwnerId() const { return m_owner; }

private:

	float m_fLifetime = 0.f;
	SmartId m_owner = InvalidLink;
};