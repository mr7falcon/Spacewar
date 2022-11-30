#pragma once

#include <map>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "IController.h"
#include "Game.h"
#include "ControllerConfiguration.h"

class CKeyboardController : public IController, public IWindowEventListener
{
public:

	CKeyboardController(const CControllerConfiguration::SConfiguration* pConfig);
	~CKeyboardController();

	virtual EControllerType GetType() const override { return Keyboard; }

	virtual void OnWindowEvent(const sf::Event& evt) override;

private:

	std::map<std::pair<sf::Event::EventType, sf::Keyboard::Key>, EControllerEvent> m_eventsMap;
};