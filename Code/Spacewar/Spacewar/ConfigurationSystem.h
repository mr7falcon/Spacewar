#pragma once

#include <filesystem>
#include <memory>

class CEntityConfiguration;

class CConfigurationSystem
{
public:

	CConfigurationSystem(const std::filesystem::path& path);
	CConfigurationSystem(const CConfigurationSystem&) = delete;
	~CConfigurationSystem();

	const CEntityConfiguration* GetEntityConfiguration() const { return m_pEntityConfiguration.get(); }

private:

	std::unique_ptr<CEntityConfiguration> m_pEntityConfiguration;
};