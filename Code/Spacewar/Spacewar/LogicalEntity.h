#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>

#include "EntitySystem.h"

class CLogicalEntity
{
public:

	CLogicalEntity() = default;

	void SetPosition(const sf::Vector2f& vPos);
	void SetRotation(float fRot);
	void SetScale(const sf::Vector2f& vScale);
	void SetVelocity(const sf::Vector2f& vVel) { m_vVel = vVel; }
	void SetAngularSpeed(float fAngSpeed) { m_fAngSpeed = fAngSpeed; }
	void SetAcceleration(const sf::Vector2f& vAccel) { m_vAccel = vAccel; }
	void SetAngularAcceleration(float fAngAccel) { m_fAngAccel = fAngAccel; }

	const sf::Vector2f& GetPosition() const { return m_vPos; }
	float GetRotation() const { return m_fRot; }
	const sf::Vector2f& GetScale() const { return m_vScale; }
	const sf::Vector2f& GetVelocity() const { return m_vVel; }
	float GetAngularSpeed() const { return m_fAngSpeed; }
	const sf::Vector2f& GetAcceleration() const { return m_vAccel; }
	float GetAngularAcceleration() const { return m_fAngAccel; }

	void Update(sf::Time dt);

private:

	void UpdateTransform();

private:

	SmartId m_physicalEntityId = InvalidLink;
	SmartId m_renderEntityId = InvalidLink;

	sf::Vector2f m_vPos;
	float m_fRot = 0.f;
	sf::Vector2f m_vScale = sf::Vector2f(1.f, 1.f);
	
	// move this in CMovementComponent?
	sf::Vector2f m_vVel;
	float m_fAngSpeed = 0.f;
	sf::Vector2f m_vAccel;
	float m_fAngAccel = 0.f;
};