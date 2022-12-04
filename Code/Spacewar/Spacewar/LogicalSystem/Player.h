#pragma once

#include "Actor.h"
#include "Controllers/Controller.h"
#include "ConfigurationSystem/PlayerConfiguration.h"

#include <memory>
#include <string>

#include <SFML\System\Time.hpp>

class CPlayer : public CActor, public IControllerEventListener
{
public:

	CPlayer(const std::string& configName, const CPlayerConfiguration::SPlayerConfiguration* pConfig);
	~CPlayer();

	void SetController(const std::shared_ptr<CController>& pController);
	const std::shared_ptr<CController>& GetController() const { return m_pController; }
	
	virtual void OnControllerEvent(EControllerEvent evt) override;

	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType_Player; }
	virtual void Update(sf::Time dt) override;
	virtual void Serialize(sf::Packet& packet, uint8_t mode, uint16_t& size) override;

	void SetShooting(bool bShoot);
	void SetAcceleration(float fAccel);
	void SetAngularSpeed(float fAngSpeed);

	int GetAmmoCount() const { return m_dAmmoCount; }
	float GetFuel() const { return m_fFuel; }
	int GetScore() const { return m_dScore; }

	void SetAmmoCount(int dAmmoCount);
	void SetShotsInBurst(int dShotsInBurst);
	void SetFuel(float fFuel);
	void SetScore(int dScore);

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
	
	int m_dShotsInBurst = 0;
	float m_fBurstCooldown = 0.f;
	float m_fShotsCooldown = 0.f;

	int m_dAmmoCount = -1;
	float m_fFuel = -1.f;
	int m_dScore = 0;
};