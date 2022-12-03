#include <thread>

#include <SFML/Window/Event.hpp>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include "Game.h"
#include "RenderSystem.h"
#include "PhysicalSystem.h"
#include "LogicalSystem.h"
#include "RenderProxy.h"
#include "ResourceSystem.h"
#include "LevelSystem.h"
#include "ConfigurationSystem.h"
#include "KeyboardController.h"
#include "ActorSystem.h"
#include "UISystem.h"
#include "Player.h"
#include "UISystem.h"
#include "NetworkSystem.h"
#include "NetworkProxy.h"
#include "SoundSystem.h"

CGame::CGame() = default;
CGame::~CGame() = default;

void CGame::Initialize()
{
#if !defined(_DEBUG) && (defined(_WIN32) || defined(_WIN64))
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	SetCurrentDirectory(L"../Game/");
#endif

	m_pResourceSystem = std::make_unique<CResourceSystem>("Resources");
	m_pConfigurationSystem = std::make_unique<CConfigurationSystem>("Configuration");
	m_pLogicalSystem = std::make_unique<CLogicalSystem>();
	m_pPhysicalSystem = std::make_unique<CPhysicalSystem>();
	m_pRenderSystem = std::make_unique<CRenderSystem>();
	m_pRenderProxy = std::make_unique<CRenderProxy>();
	m_pUISystem = std::make_unique<CUISystem>("UI");
	m_pNetworkSystem = std::make_unique<CNetworkSystem>();
	m_pNetworkProxy = std::make_unique<CNetworkProxy>();
	m_pSoundSystem = std::make_unique<CSoundSystem>();

	m_pLogicalSystem->GetLevelSystem()->CreateLevel("Menu");

	const CConfigurationSystem::SWindowConfiguration& config = m_pConfigurationSystem->GetWindowConfiguration();
	
	m_window.create(sf::VideoMode(config.resX, config.resY), "Spacewar");
	m_window.setVerticalSyncEnabled(config.bVerticalSynq);
	m_window.setFramerateLimit(config.frameLitimit);
	m_window.setActive(false);
}

void CGame::Release()
{
	m_pLogicalSystem->Release();
	m_pLogicalSystem.reset();
	m_pUISystem.reset();
	m_pNetworkProxy.reset();
	m_pNetworkSystem.reset();
	m_pConfigurationSystem.reset();
	m_pResourceSystem.reset();
}

void CGame::Start()
{
	Initialize();

	std::thread render(StartRender);

	sf::Clock frameClock;

	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();

			for (auto iter = m_windowEventListeners.begin(); iter != m_windowEventListeners.end();)
			{
				if (auto pEventListener = iter->lock())
				{
					pEventListener->OnWindowEvent(event);
					++iter;
				}
				else
				{
					iter = m_windowEventListeners.erase(iter);
				}
			}
		}

		if (m_pNetworkSystem->IsServerStarted())
		{
			m_pNetworkSystem->AcceptConnections();
			m_pNetworkSystem->ProcessClientMessages();
		}
		else if (m_pNetworkSystem->IsConnected())
		{
			m_pNetworkSystem->ProcessServerMessages();
		}

		if (!m_bPaused)
		{
			m_pPhysicalSystem->ProcessCollisions();
			m_pLogicalSystem->Update(frameClock.getElapsedTime());
		}

		frameClock.restart();

		m_pLogicalSystem->CollectGarbage();
		m_pPhysicalSystem->CollectGarbage();

		m_pSoundSystem->Update();

		m_pUISystem->Update();

		if (m_pNetworkSystem->IsServerStarted())
		{
			m_pNetworkProxy->Serialize();
		}
		
		{
			std::unique_lock<std::mutex> lock(m_renderLock);
			m_renderSync.wait(lock, [this]() { return m_bRenderComplete; });
			
			m_bRenderComplete = false;

			m_pRenderSystem->CollectGarbage();
			m_pRenderSystem->FixNumActiveEntities();
			m_pRenderProxy->SwitchStreams();

			m_bMainComplete = true;
		}
		m_renderSync.notify_one();
	}

	Release();

	render.join();
}

void CGame::StartRender()
{
	CGame& game = CGame::Get();

	game.m_window.setActive(true);

	while (game.m_window.isOpen())
	{
		{
			std::unique_lock<std::mutex> lock(game.m_renderLock);
			game.m_renderSync.wait(lock, [&]() { return game.m_bMainComplete; });

			game.m_bMainComplete = false;

			game.m_pRenderProxy->ExecuteCommands();

			game.m_window.clear(sf::Color::Black);
			game.m_pRenderSystem->Render(game.m_window);
			game.m_window.display();

			game.m_bRenderComplete = true;
		}
		game.m_renderSync.notify_one();
	}
}

void CGame::RegisterWindowEventListener(const std::weak_ptr<IWindowEventListener>& pEventListener)
{
	auto fnd = std::find_if(m_windowEventListeners.begin(), m_windowEventListeners.end(),
		[pEventListener](const std::weak_ptr<IWindowEventListener>& pIter) { return pIter.lock() == pEventListener.lock(); });
	if (fnd == m_windowEventListeners.end())
	{
		m_windowEventListeners.push_front(pEventListener);
	}
}

void CGame::Pause(bool bPause)
{
	if (bPause != m_bPaused)
	{
		if (m_bServer)
		{
			m_pNetworkProxy->BroadcastServerMessage<ServerMessage::SSetPauseMessage>(bPause);
		}
		else
		{
			m_pNetworkProxy->SendClientMessage<ClientMessage::SSetPauseMessage>(bPause);
		}
	}
	m_bPaused = bPause;
}

void CGame::ResetView(float fSize)
{
	sf::View view(sf::FloatRect(0.f, 0.f, fSize, fSize));
	m_window.setView(view);
}