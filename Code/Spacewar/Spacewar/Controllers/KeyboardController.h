#pragma once

#include "Controller.h"
#include "Game.h"
#include "ConfigurationSystem/ControllerConfiguration.h"

#include <SFML/Window/Event.hpp>

/**
 * @class CKeyboardController
 * Simple class for the transformation of the keyboard state into controller events.
 * Also used for the transformation of the window events into text entered events.
 */
class CKeyboardController : public CController, public IWindowEventListener
{
public:

	CKeyboardController(const CControllerConfiguration::SConfiguration* pConfig) : m_pConfig(pConfig) {}

	virtual EControllerType GetType() const override { return Keyboard; }
	virtual void Update() override;

	virtual void OnWindowEvent(const sf::Event& evt) override;

private:

	const CControllerConfiguration::SConfiguration* m_pConfig = nullptr;
};