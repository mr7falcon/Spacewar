#pragma once

#include <list>

enum EControllerEvent : uint8_t
{
	EControllerEvent_Invalid,
	EControllerEvent_MoveForward_Pressed,
	EControllerEvent_MoveForward_Released,
	EControllerEvent_MoveBack_Pressed,
	EControllerEvent_MoveBack_Released,
	EControllerEvent_RotatePositive_Pressed,
	EControllerEvent_RotatePositive_Released,
	EControllerEvent_RotateNegative_Pressed,
	EControllerEvent_RotateNegative_Released,
	EControllerEvent_Shoot_Pressed,
	EControllerEvent_Shoot_Released,
	EControllerEvent_Quit,
	EControllerEvent_Layout_Up = EControllerEvent_MoveForward_Pressed,
	EControllerEvent_Layout_Down = EControllerEvent_MoveBack_Pressed,
	EControllerEvent_Layout_Right = EControllerEvent_RotatePositive_Pressed,
	EControllerEvent_Layout_Left = EControllerEvent_RotateNegative_Pressed,
	EControllerEvent_Layout_Enter = EControllerEvent_Shoot_Pressed,
	EControllerEvent_Layout_Escape = EControllerEvent_Quit,
};

class IControllerEventListener
{
public:

	virtual void OnControllerEvent(EControllerEvent evt) = 0;
};

class IController			// make template class for item providers
{
public:

	virtual ~IController() = default;

	void RegisterEventListener(IControllerEventListener* pEventListener)
	{
		auto fnd = std::find(m_listeners.begin(), m_listeners.end(), pEventListener);
		if (fnd == m_listeners.end())
		{
			m_listeners.push_front(pEventListener);
		}
	}

	void UnregisterEventListener(IControllerEventListener* pEventListener)
	{
		auto fnd = std::find(m_listeners.begin(), m_listeners.end(), pEventListener);
		if (fnd != m_listeners.end())
		{
			*fnd = nullptr;
		}
	}

protected:

	std::list<IControllerEventListener*> m_listeners;
};