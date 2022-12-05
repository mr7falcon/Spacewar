#pragma once

#include <string>
#include <map>

#include "LogicalSystem.h"
#include "ConfigurationSystem/FeedbackConfiguration.h"

/**
 * @class CFeedbackSysem
 * This is a simple system which handles players' audio and visual effects.
 * Effects are created by the corresponding events, which are configured in the xml.
 */
class CFeedbackSystem
{
public:

	CFeedbackSystem() = default;
	CFeedbackSystem(const CFeedbackSystem&) = delete;

	/**
	 * @function OnEvent
	 * Spawn effects corresponding to the specified event.
	 * 
	 * @param sid - player's entity SmartId
	 * @param schema - player's feedback events configuraiton schema
	 * @param event - thrown event code
	 */
	void OnEvent(SmartId sid, const std::string& schema, CFeedbackConfiguration::FeedbackEvent event);
	
	/**
	 * @function OnEventEnd
	 * Destroy all the effects spawned by the specified event before (if they was).
	 * 
	 * @param sid - player's entity SmartId
	 * @param event - event code which effects are needed to be destroyed
	 */
	void OnEventEnd(SmartId sid, CFeedbackConfiguration::FeedbackEvent event);
	
	/**
	 * @function DestroyEffects
	 * Destroy all the player's active effects.
	 *
	 * @param sid - player's entity SmartId
	 */
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