#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

class CLogicalSystem;
class CPhysicalSystem;
class CRenderSystem;
class CRenderProxy;
class CResourceSystem;
class CConfigurationSystem;

class CGame
{
public:

	CGame(const CGame&) = delete;
	~CGame();

	static CGame& Get()
	{
		static CGame game;
		return game;
	}

	void Start();

public:

	CLogicalSystem* GetLogicalSystem() { return m_pLogicalSystem.get(); }
	CPhysicalSystem* GetPhysicalSystem() { return m_pPhysicalSystem.get(); }
	CRenderSystem* GetRenderSystem() { return m_pRenderSystem.get(); }
	CRenderProxy* GetRenderProxy() { return m_pRenderProxy.get(); }
	const CResourceSystem* GetResourceSystem() const { return m_pResourceSystem.get(); }
	const CConfigurationSystem* GetConfigurationSystem() const { return m_pConfigurationSystem.get(); }

private:

	CGame();

	void Initialize();

	static void StartRender();

private:

	std::unique_ptr<CLogicalSystem> m_pLogicalSystem;
	std::unique_ptr<CPhysicalSystem> m_pPhysicalSystem;
	std::unique_ptr<CRenderSystem> m_pRenderSystem;
	std::unique_ptr<CRenderProxy> m_pRenderProxy;
	std::unique_ptr<CResourceSystem> m_pResourceSystem;
	std::unique_ptr<CConfigurationSystem> m_pConfigurationSystem;

	std::mutex m_renderLock;
	std::condition_variable m_renderSync;
	bool m_bMainComplete = false;
	bool m_bRenderComplete = true;

	sf::RenderWindow m_window;
	sf::View m_view;
};