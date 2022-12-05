#pragma once

#include "Actor.h"

/**
 * @class CBonus
 * Bonus is an actor placed in the world, in contact with which
 * player gets some additional abstract points (fuel, ammo, etc).
 * This class describes all the bonuses' logic.
 */
class CBonus : public CActor
{
public:

	enum EBonusType : uint8_t
	{
		None,
		Ammo,
		Fuel,
	};

	CBonus(const std::string& entity);

	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType_Bonus; }
	virtual void Update(sf::Time dt) override;

	void SetBonus(EBonusType type, float fVal);
	void SetLifetime(float fLifetime);

private:

	EBonusType m_type = None;
	float m_fVal = 0.f;
	float m_fLifetime = 0.f;
};