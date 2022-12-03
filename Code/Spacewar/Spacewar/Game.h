#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>
#include <list>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML\Graphics\View.hpp>
#include <SFML/System/Time.hpp>

class IWindowEventListener
{
public:

	virtual void OnWindowEvent(const sf::Event& evt) = 0;
};

class CLogicalSystem;
class CPhysicalSystem;
class CRenderSystem;
class CRenderProxy;
class CResourceSystem;
class CConfigurationSystem;
class CUISystem;
class CNetworkSystem;
class CNetworkProxy;
class CSoundSystem;

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
	void Pause(bool pause);
	bool IsServer() const { return m_bServer; }
	void SetServer(bool bServer) { m_bServer = bServer; }
	bool IsActive() const { return m_window.hasFocus(); }

public:

	CLogicalSystem* GetLogicalSystem() { return m_pLogicalSystem.get(); }
	CPhysicalSystem* GetPhysicalSystem() { return m_pPhysicalSystem.get(); }
	CRenderSystem* GetRenderSystem() { return m_pRenderSystem.get(); }
	CRenderProxy* GetRenderProxy() { return m_pRenderProxy.get(); }
	const CResourceSystem* GetResourceSystem() const { return m_pResourceSystem.get(); }
	const CConfigurationSystem* GetConfigurationSystem() const { return m_pConfigurationSystem.get(); }
	CUISystem* GetUISystem() { return m_pUISystem.get(); }
	CNetworkSystem* GetNetworkSystem() { return m_pNetworkSystem.get(); }
	CNetworkProxy* GetNetworkProxy() { return m_pNetworkProxy.get(); }
	CSoundSystem* GetSoundSystem() { return m_pSoundSystem.get(); }

	void RegisterWindowEventListener(const std::weak_ptr<IWindowEventListener>& pEventListener);
	void ResetView(float fSize);
	
private:

	CGame();

	void Initialize();
	void Release();
	
	static void StartRender();

private:

	std::unique_ptr<CLogicalSystem> m_pLogicalSystem;
	std::unique_ptr<CPhysicalSystem> m_pPhysicalSystem;
	std::unique_ptr<CRenderSystem> m_pRenderSystem;
	std::unique_ptr<CRenderProxy> m_pRenderProxy;
	std::unique_ptr<CResourceSystem> m_pResourceSystem;
	std::unique_ptr<CConfigurationSystem> m_pConfigurationSystem;
	std::unique_ptr<CUISystem> m_pUISystem;
	std::unique_ptr<CNetworkSystem> m_pNetworkSystem;
	std::unique_ptr<CNetworkProxy> m_pNetworkProxy;
	std::unique_ptr<CSoundSystem> m_pSoundSystem;

	std::mutex m_renderLock;
	std::condition_variable m_renderSync;
	bool m_bMainComplete = false;
	bool m_bRenderComplete = true;

	sf::RenderWindow m_window;

	std::list<std::weak_ptr<IWindowEventListener>> m_windowEventListeners;

	bool m_bPaused = false;
	bool m_bServer = true;
};