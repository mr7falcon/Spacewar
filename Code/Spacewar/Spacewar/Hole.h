#pragma once

#include "Actor.h"

class CHole : public CActor
{
public:

	CHole();

	void SetGravityForce(float fGravityForce) { m_fGravityForce = fGravityForce; }

	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType::Hole; }
	virtual void Update(sf::Time dt) override;

private:

	float m_fGravityForce = 0.f;
};