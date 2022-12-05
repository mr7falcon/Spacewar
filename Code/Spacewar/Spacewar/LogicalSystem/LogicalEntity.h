#pragma once

#include "EntitySystem.h"

#include <vector>

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>

class CPhysicalEntity;
class CRenderEntity;

/**
 * @class CLogicalEntity
 * Logical entity provides the base 2d game world functionalities:
 * moving, rotating and scaling. It also obtain can own a physical
 * entity and a few render entities (render slots), which are
 * synced with the parent logical entity.
 */
class CLogicalEntity : public CEntity, private sf::Transformable
{
public:

	struct SRenderSlot
	{
		int textureId;
		sf::Vector2f vSize;
	};

	CLogicalEntity() = default;

	void SetPhysics(SmartId sid) { m_physicalEntityId = sid; }
	void SetRender(SmartId sid) { m_renderEntityId = sid; }

	void AddRenderSlot(const SRenderSlot& slot);

	/**
	 * @function ActivateRenderSlot
	 * Activate the render slot with the corresponding index.
	 * Replaces the owned render entity's texture with the new one
	 * in respect with the slot configuration.
	 * 
	 * @param slot - slot index to activate.
	 */
	void ActivateRenderSlot(int slot);

	int GetActiveRenderSlot() const { return m_dActiveRenderSlot; }

	// Functions changing the current entity transform.
	// They are also trigger callbacks which transform the owned physical and render entities.
	void SetPosition(const sf::Vector2f& vPos);
	void SetRotation(float fRot);
	void SetScale(float fScale);

	void SetVelocity(const sf::Vector2f& vVel) { m_vVel = vVel; }
	void SetAngularSpeed(float fAngSpeed) { m_fAngSpeed = fAngSpeed; }

	const sf::Vector2f& GetPosition() const { return getPosition(); }
	float GetRotation() const { return getRotation(); }
	float GetScale() const { return getScale().x; }
	const sf::Transform& GetTransform() const { return getTransform(); }

	const sf::Vector2f& GetVelocity() const { return m_vVel; }
	float GetAngularSpeed() const { return m_fAngSpeed; }
	sf::Vector2f GetForwardDirection() const;

	SmartId GetPhysicalEntityId() { return m_physicalEntityId; }
	SmartId GetRenderEntityId() { return m_renderEntityId; }
	
	/**
	 * @function Update
	 * This function performs moving and rotating of the entity
	 * accordingly with the set velocity and angular speed.
	 * Should be called every frame.
	 * 
	 * @param dt - time since the last function call.
	 */
	void Update(sf::Time dt);

private:

	void OnTransformUpdated();

private:

	SmartId m_physicalEntityId = InvalidLink;
	SmartId m_renderEntityId = InvalidLink;
	
	sf::Vector2f m_vVel;
	float m_fAngSpeed = 0.f;

	int m_dActiveRenderSlot = 0;
	std::vector<SRenderSlot> m_renderSlots;
};