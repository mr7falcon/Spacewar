#pragma once

#include <string>
#include <map>

#include "LogicalSystem.h"
#include "ConfigurationSystem/FeedbackConfiguration.h"

class CFeedbackSystem
{
public:

	CFeedbackSystem() = default;
	CFeedbackSystem(const CFeedbackSystem&) = delete;

	void OnEvent(SmartId sid, const std::string& schema, CFeedbackConfiguration::FeedbackEvent event);
	void OnEventEnd(SmartId sid, CFeedbackConfiguration::FeedbackEvent event);
	void DestroyEvents(SmartId sid);

private:

	int ActivateRenderSlot(SmartId sid, int slot);
	int PlaySound(int resourceId, bool bCicled, float fPitch, int volume);
	void StopSound(int soundId);

private:

	struct SActiveEvent
	{
		int renderSlot = 0;
		int soundId = -1;
	};
	std::map<std::pair<SmartId, CFeedbackConfiguration::FeedbackEvent>, SActiveEvent> m_activeEvents;
};