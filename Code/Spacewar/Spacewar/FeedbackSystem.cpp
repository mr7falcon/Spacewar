#include "FeedbackSystem.h"
#include "Game.h"
#include "RenderSystem.h"
#include "RenderProxy.h"
#include "SoundSystem.h"
#include "ConfigurationSystem.h"

int CFeedbackSystem::ActivateRenderSlot(SmartId sid, int slot)
{
	int currentSlot = 0;
	if (CLogicalEntity* pLogicalEntity = CGame::Get().GetLogicalSystem()->GetEntity(sid))
	{
		currentSlot = pLogicalEntity->GetActiveRenderSlot();
		if (slot >= 0)
		{
			pLogicalEntity->ActivateRenderSlot(slot);
		}
	}
	return currentSlot;
}

int CFeedbackSystem::PlaySound(int resourceId, bool bCicled, float fPitch, int volume)
{
	int soundId = CGame::Get().GetSoundSystem()->PlaySound(resourceId, bCicled);
	if (sf::Sound* pSound = CGame::Get().GetSoundSystem()->GetSound(soundId))
	{
		pSound->setPitch(fPitch);
		pSound->setVolume((float)volume);
	}
	return soundId;
}

void CFeedbackSystem::StopSound(int soundId)
{
	CGame::Get().GetSoundSystem()->StopSound(soundId);
}

void CFeedbackSystem::OnEvent(SmartId sid, const std::string& schema, CFeedbackConfiguration::FeedbackEvent event)
{
	auto fnd = m_activeEvents.find(std::make_pair(sid, event));
	if (fnd != m_activeEvents.end())
	{
		return;
	}

	if (auto* pConfig = CGame::Get().GetConfigurationSystem()->GetFeedbackConfiguration()->GetConfiguration(schema, event))
	{
		SActiveEvent activeEvent;
		activeEvent.renderSlot = ActivateRenderSlot(sid, pConfig->renderSlot);
		activeEvent.soundId = PlaySound(pConfig->soundId, pConfig->bContinuous, pConfig->fSoundPitch, pConfig->soundVolume);

		if (pConfig->bContinuous)
		{
			m_activeEvents.emplace(std::make_pair(sid, event), std::move(activeEvent));
		}
	}
}

void CFeedbackSystem::OnEventEnd(SmartId sid, CFeedbackConfiguration::FeedbackEvent event)
{
	auto fnd = m_activeEvents.find(std::make_pair(sid, event));
	if (fnd != m_activeEvents.end())
	{
		ActivateRenderSlot(sid, fnd->second.renderSlot);
		StopSound(fnd->second.soundId);
		m_activeEvents.erase(fnd);
	}
}

void CFeedbackSystem::DestroyEvents(SmartId sid)
{
	for (int i = CFeedbackConfiguration::EventsFirst; i < CFeedbackConfiguration::EventsCount; ++i)
	{
		OnEventEnd(sid, (CFeedbackConfiguration::FeedbackEvent)i);
	}
}