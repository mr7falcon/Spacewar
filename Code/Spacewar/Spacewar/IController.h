#pragma once

#include <list>
#include <vector>
#include <functional>

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

class IControllerEventListener
{
public:

	virtual void OnControllerEvent(EControllerEvent evt) = 0;
	virtual void OnTextEntered(char chr) {}
};

class IController			// make template class for item providers
{
public:

	enum EControllerType
	{
		Keyboard,
		Gamepad,
		Network
	};

	virtual ~IController() = default;

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