#pragma once

#include "Actor.h"
#include "Controllers/Controller.h"
#include "ConfigurationSystem/PlayerConfiguration.h"

#include <memory>
#include <string>

#include <SFML\System\Time.hpp>

/**
 * @class CPlayer
 * Player is an actor which is controller by the actual humans' players.
 * Generally, the player can accelerate, rotate and shoot. Some of these
 * actions require specific logic like ammos or fuel. This class describes all the players' logic.
 */
class CPlayer : public CActor, public IControllerEventListener
{
public:

	CPlayer(const std::string& configName, const CPlayerConfiguration::SPlayerConfiguration* pConfig);
	~CPlayer();

	/**
	 * @function SetController
	 * Link the controller with the player. The player stores and updates
	 * the controller which it owns to receive some events.
	 * 
	 * @param pController - controller to be linked with the player.
	 */
	void SetController(const std::shared_ptr<CController>& pController);
	const std::shared_ptr<CController>& GetController() const { return m_pController; }
	
	/**
	 * @function OnControllerEvent
	 * Inherited function to handle the controller events.
	 * 
	 * @evt - controller event's code
	 */
	virtual void OnControllerEvent(EControllerEvent evt) override;

	/**
	 * @function OnCollision
	 * Inherited function to handle the collisions.
	 * 
	 * @param sid - SmartId of the logical entity which are the player collided with.
	 */
	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType_Player; }
	virtual void Update(sf::Time dt) override;

	/**
	 * @function Serialize
	 * Serialize the current player's state to replicate it on the client side.
	 *
	 * @param packet - network data packet (received or to send).
	 * @param mode - serialization mode (read, write, count).
	 * @param size - output size of the actor serialization structure.
	 */
	virtual void Serialize(sf::Packet& packet, uint8_t mode, uint16_t& size) override;

	void SetShooting(bool bShoot);
	void SetAcceleration(float fAccel);
	void SetAngularSpeed(float fAngSpeed);

	int GetAmmoCount() const { return m_ammoCount; }
	float GetFuel() const { return m_fFuel; }
	int GetScore() const { return m_score; }

	void SetAmmoCount(int ammoCount);
	void SetShotsInBurst(int shotsInBurst);
	void SetFuel(float fFuel);
	void SetScore(int score);

	const std::string& GetConfigName() const { return m_configName; }
	bool IsLocal() const { return m_pController ? m_pController->GetType() != CController::Network : false; }

private:

	bool CanShoot() const;
	void Shoot();

private:

	const std::string m_configName;
	const CPlayerConfiguration::SPlayerConfiguration* m_pConfig = nullptr;

	std::shared_ptr<CController> m_pController;

	float m_fAccel = 0.f;
	
	bool m_bShooting = false;
	
	int m_shotsInBurst = 0;
	float m_fBurstCooldown = 0.f;
	float m_fShotsCooldown = 0.f;

	int m_ammoCount = -1;
	float m_fFuel = -1.f;
	int m_score = 0;
};