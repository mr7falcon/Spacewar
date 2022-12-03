#include "FeedbackConfiguration.h"
#include "Game.h"

CFeedbackConfiguration::CFeedbackConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		std::cout << "Failed to load feedback configuration: " << res.description() << std::endl;
		return;
	}

	auto root = doc.child("Schemas");
	if (!root)
	{
		std::cout << "Invalid root element in feedback configuration" << std::endl;
		return;
	}

	for (auto iter = root.begin(); iter != root.end(); ++iter)
	{
		std::string name = iter->attribute("name").value();
		if (!name.empty())
		{
			auto fnd = m_feedbackSchemas.find("name");
			if (fnd != m_feedbackSchemas.end())
			{
				std::cout << "Feedback schema with name " << name << " already exists" << std::endl;
				continue;
			}

			std::vector<SFeedbackConfiguration> events(EventsCount);

			events[Move] = LoadFeedback(iter->child("Move"));
			events[Death] = LoadFeedback(iter->child("Death"));
			events[Shoot] = LoadFeedback(iter->child("Shoot"));

			m_feedbackSchemas.emplace(std::move(name), std::move(events));
		}
	}
}

CFeedbackConfiguration::SFeedbackConfiguration CFeedbackConfiguration::LoadFeedback(const pugi::xml_node& node)
{
	SFeedbackConfiguration feedback;
	if (node)
	{
		feedback.bContinuous = node.attribute("continuous").as_bool();
		feedback.renderSlot = node.attribute("renderSlot").as_int(feedback.renderSlot);
		feedback.soundId = CGame::Get().GetResourceSystem()->GetSoundId(node.attribute("sound").value());
		feedback.fSoundPitch = node.attribute("pitch").as_float(feedback.fSoundPitch);
		feedback.soundVolume = node.attribute("volume").as_int(feedback.soundVolume);
	}
	return feedback;
}

const CFeedbackConfiguration::SFeedbackConfiguration* CFeedbackConfiguration::GetConfiguration(const std::string& schema, FeedbackEvent event) const
{
	if (event != EventsCount)
	{
		auto fnd = m_feedbackSchemas.find(schema);
		if (fnd != m_feedbackSchemas.end())
		{
			return &fnd->second[event];
		}
	}
	return nullptr;
}