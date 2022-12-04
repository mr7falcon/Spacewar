#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include <SFML/Graphics/Color.hpp>

class CEntityConfiguration;
class CControllerConfiguration;
class CLevelConfiguration;
class CPlayerConfiguration;
class CFeedbackConfiguration;

class CConfigurationSystem
{
public:

	CConfigurationSystem(const std::filesystem::path& path);
	CConfigurationSystem(const CConfigurationSystem&) = delete;
	~CConfigurationSystem();

	struct SWindowConfiguration
	{
		int resX = 1000;
		int resY = 1000;
		bool bVerticalSynq = true;
		int frameLitimit = 60;
	};

	const CEntityConfiguration* GetEntityConfiguration() const { return m_pEntityConfiguration.get(); }
	const CControllerConfiguration* GetControllerConfiguration() const { return m_pControllerConfiguration.get(); }
	const CLevelConfiguration* GetLevelConfiguration() const { return m_pLevelConfiguration.get(); }
	const CPlayerConfiguration* GetPlayerConfiguration() const { return m_pPlayerConfiguration.get(); }
	const CFeedbackConfiguration* GetFeedbackConfiguration() const { return m_pFeedbackConfiguration.get(); }
	const SWindowConfiguration& GetWindowConfiguration() const { return m_windowConfiguration; }

public:

	static sf::Color ParseColor(const std::string& color);

	void LoadWindowConfiguration(const std::filesystem::path& path);

private:

	std::unique_ptr<CEntityConfiguration> m_pEntityConfiguration;
	std::unique_ptr<CControllerConfiguration> m_pControllerConfiguration;
	std::unique_ptr<CLevelConfiguration> m_pLevelConfiguration;
	std::unique_ptr<CPlayerConfiguration> m_pPlayerConfiguration;
	std::unique_ptr<CFeedbackConfiguration> m_pFeedbackConfiguration;
	SWindowConfiguration m_windowConfiguration;
};