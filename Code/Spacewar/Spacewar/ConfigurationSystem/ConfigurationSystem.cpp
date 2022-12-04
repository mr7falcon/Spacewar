#include "StdAfx.h"
#include "Game.h"
#include "ConfigurationSystem.h"
#include "EntityConfiguration.h"
#include "ControllerConfiguration.h"
#include "LevelConfiguration.h"
#include "PlayerConfiguration.h"
#include "FeedbackConfiguration.h"

CConfigurationSystem::CConfigurationSystem(const std::filesystem::path& path)
	: m_pEntityConfiguration(std::make_unique<CEntityConfiguration>(path / "Entities.xml"))
	, m_pControllerConfiguration(std::make_unique<CControllerConfiguration>(path / "Controller.xml"))
	, m_pLevelConfiguration(std::make_unique<CLevelConfiguration>(path / "Levels.xml"))
	, m_pPlayerConfiguration(std::make_unique<CPlayerConfiguration>(path / "Players.xml"))
	, m_pFeedbackConfiguration(std::make_unique<CFeedbackConfiguration>(path / "Feedback.xml"))
{
	LoadWindowConfiguration(path / "Window.xml");
}

void CConfigurationSystem::LoadWindowConfiguration(const std::filesystem::path& path)
{
	pugi::xml_document doc;

	auto res = doc.load_file(path.c_str());
	if (!res)
	{
		Log("Failed to load window configuration: ", res.description());
		return;
	}

	auto root = doc.child("Window");
	if (!root)
	{
		Log("Invalid root element in window configuration");
		return;
	}

	m_windowConfiguration.resX = root.attribute("resX").as_int(m_windowConfiguration.resX);
	m_windowConfiguration.resY = root.attribute("resY").as_int(m_windowConfiguration.resY);
	m_windowConfiguration.bVerticalSynq = root.attribute("verticalSynq").as_bool(m_windowConfiguration.bVerticalSynq);
	m_windowConfiguration.frameLitimit = root.attribute("frameLimit").as_int(m_windowConfiguration.frameLitimit);
}

CConfigurationSystem::~CConfigurationSystem() = default;

sf::Color CConfigurationSystem::ParseColor(const std::string& color)
{
	if (color == "White")
		return sf::Color::White;
	if (color == "Black")
		return sf::Color::Black;
	if (color == "Red")
		return sf::Color::Red;
	if (color == "Blue")
		return sf::Color::Blue;
	if (color == "Green")
		return sf::Color::Green;
	if (color == "Yellow")
		return sf::Color::Yellow;
	return sf::Color::Black;
}