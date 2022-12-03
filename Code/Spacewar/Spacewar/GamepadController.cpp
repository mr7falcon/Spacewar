#include <SFML/Window/Joystick.hpp>

#include "GamepadController.h"
#include "Game.h"

void CGamepadController::Update()
{
	static constexpr int ForwardButton = 0;
	static constexpr int ShootButton = 1;
	static constexpr int EscapeButton = 7;
	static constexpr float RotatePositiveMinX = 20.f;
	static constexpr float RotateNegativeMinX = -20.f;

	if (!CGame::Get().IsActive())
	{
		return;
	}

	std::vector<EControllerEvent> events;

	auto checkJoystick = [this](int key) { return sf::Joystick::isButtonPressed(m_idx, key); };

	CheckButton<int>(checkJoystick, ForwardButton, EControllerEvent_MoveForward_Pressed, EControllerEvent_MoveForward_Released, events);
	CheckButton<int>(checkJoystick, ShootButton, EControllerEvent_Shoot_Pressed, EControllerEvent_Shoot_Released, events);
	CheckButton<int>(checkJoystick, EscapeButton, EControllerEvent_Quit, events);

	float x = sf::Joystick::getAxisPosition(m_idx, sf::Joystick::X);
	if (x >= RotatePositiveMinX)
	{
		events.push_back(EControllerEvent_RotatePositive_Pressed);
	}
	else if (x <= RotateNegativeMinX)
	{
		events.push_back(EControllerEvent_RotateNegative_Pressed);
	}
	else
	{
		events.push_back(EControllerEvent_Rotate_Released);
	}
	
	SendEvents(std::move(events));
}