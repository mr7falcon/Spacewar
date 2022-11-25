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

private:

	float m_fLifetime = 0.f;
};