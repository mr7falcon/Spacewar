#include <iostream>

#include <SFML/Window/Event.hpp>

#include "KeyboardController.h"
#include "ConfigurationSystem.h"
#include "Player.h"

CKeyboardController::CKeyboardController(const std::string& schema)
{
	const auto* pConfig = CGame::Get().GetConfigurationSystem()->GetControllerConfiguration()->GetConfiguration(schema);
	if (!pConfig)
	{
		std::cout << "Invalid schema name specified " << schema << std::endl;
		return;
	}

	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->moveForward)] = EControllerEvent_MoveForward_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->moveForward)] = EControllerEvent_MoveForward_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->rotatePositive)] = EControllerEvent_RotatePositive_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->rotatePositive)] = EControllerEvent_RotatePositive_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->rotateNegative)] = EControllerEvent_RotateNegative_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->rotateNegative)] = EControllerEvent_RotateNegative_Released;
	m_eventsMap[std::make_pair(sf::Event::KeyPressed, pConfig->shoot)] = EControllerEvent_Shoot_Pressed;
	m_eventsMap[std::make_pair(sf::Event::KeyReleased, pConfig->shoot)] = EControllerEvent_Shoot_Released;

	CGame::Get().RegisterWindowEventListener(this);
}

CKeyboardController::~CKeyboardController()
{
	CGame::Get().UnregisterWindowEventListener(this);
}

void CKeyboardController::OnWindowEvent(const sf::Event& evt)
{
	if (!m_pControllerPlayer)
	{
		return;
	}

	if (evt.type == sf::Event::KeyPressed || evt.type == sf::Event::KeyReleased)
	{
		auto fnd = m_eventsMap.find(std::make_pair(evt.type, evt.key.code));
		if (fnd != m_eventsMap.end())
		{
			m_pControllerPlayer->OnControllerEvent(fnd->second);
		}
	}
}