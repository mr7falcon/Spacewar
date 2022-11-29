#include <iostream>

#include <SFML/Window/Event.hpp>

#include "KeyboardController.h"
#include "Player.h"

CKeyboardController::CKeyboardController(const CControllerConfiguration::SConfiguration* pConfig)
{
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->moveForward)] = EControllerEvent_MoveForward_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->moveForward)] = EControllerEvent_MoveForward_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->moveBack)] = EControllerEvent_MoveBack_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->moveBack)] = EControllerEvent_MoveBack_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->rotatePositive)] = EControllerEvent_RotatePositive_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->rotatePositive)] = EControllerEvent_RotatePositive_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->rotateNegative)] = EControllerEvent_RotateNegative_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->rotateNegative)] = EControllerEvent_RotateNegative_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->shoot)] = EControllerEvent_Shoot_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->shoot)] = EControllerEvent_Shoot_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->escape)] = EControllerEvent_Quit;

	CGame::Get().RegisterWindowEventListener(this);
}

CKeyboardController::~CKeyboardController()
{
	CGame::Get().UnregisterWindowEventListener(this);
}

void CKeyboardController::OnWindowEvent(const sf::Event& evt)
{
	if (evt.type == sf::Event::KeyPressed || evt.type == sf::Event::KeyReleased)
	{
		auto fnd = m_eventsMap.find(std::make_pair(evt.type, evt.key.code));
		if (fnd != m_eventsMap.end())
		{
			for (auto iter = m_listeners.begin(); iter != m_listeners.end();)
			{
				if (*iter == nullptr)
				{
					iter = m_listeners.erase(iter);
				}
				else
				{
					(*iter)->OnControllerEvent(fnd->second);
					++iter;
				}
			}
		}
	}
}