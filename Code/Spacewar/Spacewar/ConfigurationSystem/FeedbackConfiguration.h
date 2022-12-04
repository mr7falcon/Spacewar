#pragma once

#include "ResourceSystem.h"

#include <map>
#include <filesystem>

#include <pugixml.hpp>

class CFeedbackConfiguration
{
public:

	enum FeedbackEvent
	{
		EventsFirst = 0,
		Move = EventsFirst,
		Shoot,
		Death,
		EventsCount,
	};

	struct SFeedbackConfiguration
	{
		bool bContinuous = false;
		int renderSlot = -1;
		int soundId = InvalidResourceId;
		float fSoundPitch = 1.f;
		int soundVolume = 100;
	};

	CFeedbackConfiguration(const std::filesystem::path& path);
	CFeedbackConfiguration(const CFeedbackConfiguration&) = delete;

	const SFeedbackConfiguration* GetConfiguration(const std::string& schema, FeedbackEvent event) const;

private:

	SFeedbackConfiguration LoadFeedback(const pugi::xml_node& node);

private:

	std::map<std::string, std::vector<SFeedbackConfiguration>> m_feedbackSchemas;
};