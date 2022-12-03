#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "IController.h"
#include "Game.h"
#include "ControllerConfiguration.h"

class CKeyboardController : public IController, public IWindowEventListener
{
public:

	CKeyboardController(const CControllerConfiguration::SConfiguration* pConfig) : m_pConfig(pConfig) {}

	virtual EControllerType GetType() const override { return Keyboard; }
	virtual void Update() override;

	virtual void OnWindowEvent(const sf::Event& evt) override;

private:

	const CControllerConfiguration::SConfiguration* m_pConfig = nullptr;
};