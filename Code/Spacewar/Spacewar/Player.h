#pragma once

#include <memory>
#include <string>

#include <SFML\System\Time.hpp>

#include "Actor.h"
#include "IController.h"
#include "PlayerConfiguration.h"

class CPlayer : public CActor
{
public:

	CPlayer(const CPlayerConfiguration::SPlayerConfiguration* pConfig);

	void SetController(std::unique_ptr<IController> pController);
	void OnControllerEvent(EControllerEvent evt);

	virtual void OnCollision(SmartId sid) override;
	virtual EActorType GetType() const override { return EActorType::Player; }
	virtual void Update(sf::Time dt) override;

	int GetAmmoCount() const { return m_dAmmoCount; }
	float GetFuel() const { return m_fFuel; }

	void SetAmmoCount(int dAmmoCount) { m_dAmmoCount = dAmmoCount; }
	void SetFuel(float fFuel) { m_fFuel = fFuel; }

private:

	bool CanShoot() const;
	void Shoot();

private:

	const CPlayerConfiguration::SPlayerConfiguration* m_pConfig = nullptr;

	std::unique_ptr<IController> m_pController;

	float m_fAccel = 0.f;
	
	bool m_bShooting = false;
	
	int m_dShotsInBurst = 0;
	float m_fBurstCooldown = 0.f;
	float m_fShotsCooldown = 0.f;

	int m_dAmmoCount = -1;
	float m_fFuel = -1.f;
};