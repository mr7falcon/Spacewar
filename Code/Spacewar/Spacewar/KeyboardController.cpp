#include <iostream>

#include <SFML/Window/Keyboard.hpp>

#include "KeyboardController.h"
#include "Player.h"

void CKeyboardController::Update()
{
	if (!m_pConfig || !CGame::Get().IsActive())
	{
		return;
	}

	std::vector<EControllerEvent> events;

	CheckButton<sf::Keyboard::Key>(sf::Keyboard::isKeyPressed, m_pConfig->moveForward, EControllerEvent_MoveForward_Pressed, EControllerEvent_MoveForward_Released, events);
	CheckButton<sf::Keyboard::Key>(sf::Keyboard::isKeyPressed, m_pConfig->moveBack, EControllerEvent_MoveBack_Pressed, EControllerEvent_MoveBack_Released, events);
	CheckButton<sf::Keyboard::Key>(sf::Keyboard::isKeyPressed, m_pConfig->shoot, EControllerEvent_Shoot_Pressed, EControllerEvent_Shoot_Released, events);
	CheckButton<sf::Keyboard::Key>(sf::Keyboard::isKeyPressed, m_pConfig->escape, EControllerEvent_Quit, events);
	CheckButton<sf::Keyboard::Key>(sf::Keyboard::isKeyPressed, m_pConfig->rotatePositive, m_pConfig->rotateNegative, 
		EControllerEvent_RotatePositive_Pressed, EControllerEvent_RotateNegative_Pressed, EControllerEvent_Rotate_Released, events);

	SendEvents(std::move(events));
}

void CKeyboardController::OnWindowEvent(const sf::Event& evt)
{
	if (evt.type == sf::Event::TextEntered)
	{
		ForEachListener([evt](IControllerEventListener* pEventListener) { pEventListener->OnTextEntered(evt.text.unicode); });
	}
}