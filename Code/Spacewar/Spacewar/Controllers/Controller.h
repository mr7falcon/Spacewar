#pragma once

#include "StdAfx.h"

enum EControllerEvent : uint8_t
{
	EControllerEvent_Invalid,
	EControllerEvent_MoveForward_Pressed,
	EControllerEvent_MoveForward_Released,
	EControllerEvent_MoveBack_Pressed,
	EControllerEvent_MoveBack_Released,
	EControllerEvent_RotatePositive_Pressed,
	EControllerEvent_RotateNegative_Pressed,
	EControllerEvent_Rotate_Released,
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

/**
 * @interface IControllerEventListener
 * Interface for the controllers' events processing.
 * Each IControllerEventListener must be registered in the corresponding
 * CController instance and unregistered before the pointer invalidation.
 */
class IControllerEventListener
{
public:

	/**
	 * @function OnControllerEvent
	 * This method are triggered on any controller event.
	 * Listener must override it to handle the events.
	 */
	virtual void OnControllerEvent(EControllerEvent evt) = 0;
	
	/**
	 * @function OnTextEntered
	 * This method are triggered when the window received some text entered.
	 * Listener must override it to handle the text input.
	 */
	virtual void OnTextEntered(char chr) {}
};

/**
 * @class CController
 * Base class for all the controllers, storing events listeners
 * and some additional useful functions for them.
 */
class CController
{
public:

	enum EControllerType
	{
		Keyboard,
		Gamepad,
		Network
	};

	virtual ~CController() = default;

	virtual EControllerType GetType() const = 0;
	virtual void Update() {}

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
	
	/**
	 * @function ForEachListener
	 * Function to iterate over the event listeners. Also safely remove the invalidated listeners.
	 * 
	 * @param f - processing function which the event listeners is applied to.
	 */
	inline void ForEachListener(std::function<void(IControllerEventListener*)> f)
	{
		for (auto iter = m_listeners.begin(); iter != m_listeners.end();)
		{
			if (*iter == nullptr)
			{
				iter = m_listeners.erase(iter);
			}
			else
			{
				f(*iter);
				++iter;
			}
		}
	}

	/**
	 * @function SendEvents
	 * Iterate over the events vector and process them by the listeners.
	 * 
	 * @template param T - must always be a vector of EControllerEvent.
	 * Declared only for the universal reference usage to avoid double implementation.
	 * @param events - vector of the new events to handle.
	 * @note Repeated events are not handled.
	 */
	template <typename T = std::vector<EControllerEvent>>
	inline void SendEvents(T&& events)
	{
		for (EControllerEvent event : events)
		{
			auto fnd = std::find(m_lastEvents.begin(), m_lastEvents.end(), event);
			if (fnd == m_lastEvents.end())
			{
				ForEachListener([event](IControllerEventListener* pEventListener) { pEventListener->OnControllerEvent(event); });
			}
		}
		m_lastEvents = std::forward<std::vector<EControllerEvent>>(events);
	}

	/**
	 * @functions CheckButton
	 * Three functions to check if the button is pressed with some different conditional cases.
	 * 
	 * @template param T - parameter type for the state-checking function corresponding to the specific controller.
	 * @param f - function, checking if the button is pressed.
	 * @params cond<n> - conditional parameter for the checking function.
	 * @params br<n> - events, corresponding to the specific checking branch.
	 * @events - events vector to add the new ones.
	 */
	template <typename T>
	inline bool CheckButton(std::function<bool(T)> f, T cond1, EControllerEvent br1, std::vector<EControllerEvent>& events)
	{
		if (f(cond1))
		{
			events.push_back(br1);
			return true;
		}
		return false;
	}

	template <typename T>
	inline void CheckButton(std::function<bool(T)> f, T cond1, EControllerEvent br1, EControllerEvent br2, std::vector<EControllerEvent>& events)
	{
		if (!CheckButton(f, cond1, br1, events))
		{
			events.push_back(br2);
		}
	}

	template <typename T>
	inline void CheckButton(std::function<bool(T)> f, T cond1, T cond2, EControllerEvent br1, EControllerEvent br2, EControllerEvent br3, std::vector<EControllerEvent>& events)
	{
		if (!CheckButton(f, cond1, br1, events) && !CheckButton(f, cond2, br2, events))
		{
			events.push_back(br3);
		}
	}

protected:

	std::list<IControllerEventListener*> m_listeners;
	std::vector<EControllerEvent> m_lastEvents;
};