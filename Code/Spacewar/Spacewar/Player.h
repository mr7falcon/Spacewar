#pragma once

#include <memory>

#include <SFML\System\Time.hpp>

#include "Actor.h"
#include "IController.h"

class CPlayer : public CActor
{
public:

	CPlayer();

	void SetController(std::unique_ptr<IController> pController);
	void OnControllerEvent(EControllerEvent evt);

	virtual bool IsPlayer() const override { return true; }
	virtual void Update(sf::Time dt) override;

private:

	std::unique_ptr<IController> m_pController;

	float m_fAccel = 0.f;
};