#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>
#include <list>
#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

/**
 * @interface IWindowEventListener
 * Interface for the game window events processing.
 * Each IWeaponEventListener must be registered in CGame instance
 * using RegisterWindowEventListener method. Unregistering listeners
 * after their removal is provided by CGame.
 */
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

/**
 * @class CGame
 * The main game class. Carries out the game loop
 * and communication between the game systems.
 * CGame is a singletone and could be accessed from any game system.
 */
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


	// Start the game. Initialize the main game systems and run the game loop.
	void Start();

	// Pause the game. Paused game doesn't update physical and logical systems.
	void Pause(bool pause);
	
	/**
	 * @function IsServer
	 * @function SetServer
	 * Get and set this client's server status. Most of the game logic
	 * in online mode is carried out on the server, after which the state
	 * of the objects of the game world is serialized to all connected clients.
	 * For more information see CNetworkSystem and CNetworkProxy.
	 */
	bool IsServer() const { return m_bServer; }
	void SetServer(bool bServer) { m_bServer = bServer; }
	
	// Check if the game window is in focus now. While the window is inactive
	// controllers don't provide any events.
	bool IsActive() const { return m_window.hasFocus(); }

public:

	// All the game systems getters

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

	// The constructor is private, but it is possible to
	// create and get the game instance via the static Get method
	CGame();

	// Initialize the game systems in the proper order
	void Initialize();

	// Release the game systems in the proper order
	void Release();

	// Process the game window evetns and send them to event listeners
	void ProcessEvents();
	
	/**
	 * @function StartRender
	 * Start the render thread logic, that includes processing render
	 * commands from the Main thread and displaying game objects in the window.
	 * Synchronization is guaranteed by the dual buffer and the safe entity
	 * container algorithms.
	 * For more information see CRenderSystem and CRenderProxy.
	 */
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