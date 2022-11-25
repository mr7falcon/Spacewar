#pragma once

enum EControllerEvent : uint8_t
{
	EControllerEvent_MoveForward_Pressed,
	EControllerEvent_MoveForward_Released,
	EControllerEvent_RotatePositive_Pressed,
	EControllerEvent_RotatePositive_Released,
	EControllerEvent_RotateNegative_Pressed,
	EControllerEvent_RotateNegative_Released,
	EControllerEvent_Shoot_Pressed,
	EControllerEvent_Shoot_Released,
};

class CPlayer;

class IController
{
public:

	virtual ~IController() = default;

	void SetControlledPlayer(CPlayer* pPlayer) { m_pControllerPlayer = pPlayer; }

protected:

	CPlayer* m_pControllerPlayer = nullptr;
};