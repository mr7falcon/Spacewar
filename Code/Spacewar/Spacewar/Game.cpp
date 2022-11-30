#include <thread>

#include <SFML/Window/Event.hpp>

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

CGame::CGame() = default;
CGame::~CGame() = default;

void CGame::Initialize()
{
	m_pResourceSystem = std::make_unique<CResourceSystem>("Resources");
	m_pConfigurationSystem = std::make_unique<CConfigurationSystem>("Configuration");
	m_pLogicalSystem = std::make_unique<CLogicalSystem>();
	m_pPhysicalSystem = std::make_unique<CPhysicalSystem>();
	m_pRenderSystem = std::make_unique<CRenderSystem>();
	m_pRenderProxy = std::make_unique<CRenderProxy>();
	m_pUISystem = std::make_unique<CUISystem>("UI");
	m_pNetworkSystem = std::make_unique<CNetworkSystem>();

	m_pLogicalSystem->GetLevelSystem()->CreateLevel("Menu");
	
	float levelSize = m_pLogicalSystem->GetLevelSystem()->GetLevelSize();
	m_view.reset(sf::FloatRect(0.f, 0.f, levelSize, levelSize));
	
	m_window.create(sf::VideoMode(900, 900), "Spacewar");
	m_window.setView(m_view);
	m_window.setActive(false);
}

void CGame::Start()
{
	Initialize();

	std::thread render(StartRender);

	m_pNetworkSystem->StartServer();

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
				if (*iter == nullptr)
				{
					iter = m_windowEventListeners.erase(iter);
				}
				else
				{
					(*iter)->OnWindowEvent(event);
					++iter;
				}
			}
		}

		m_pNetworkSystem->ProcessMessages();

		if (!m_bPaused)
		{
			m_pPhysicalSystem->ProcessCollisions();
			m_pLogicalSystem->Update(frameClock.getElapsedTime());
		}

		frameClock.restart();

		m_pLogicalSystem->CollectGarbage();
		m_pPhysicalSystem->CollectGarbage();

		m_pUISystem->Update();

		m_pNetworkSystem->SerializeActors();
		
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

	m_pLogicalSystem->GetLevelSystem()->ClearLevel();
	m_pUISystem->Release();
	m_pNetworkSystem->SetVirtualController(nullptr);

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

void CGame::RegisterWindowEventListener(IWindowEventListener* pEventListener)
{
	auto fnd = std::find(m_windowEventListeners.begin(), m_windowEventListeners.end(), pEventListener);
	if (fnd == m_windowEventListeners.end())
	{
		m_windowEventListeners.push_front(pEventListener);
	}
}

void CGame::UnregisterWindowEventListener(IWindowEventListener* pEventListener)
{
	auto fnd = std::find(m_windowEventListeners.begin(), m_windowEventListeners.end(), pEventListener);
	if (fnd != m_windowEventListeners.end())
	{
		*fnd = nullptr;
	}
}

void CGame::Pause(bool bPause)
{
	if (m_bServer && bPause != m_bPaused)
	{
		m_pNetworkSystem->SendPause(bPause);
	}
	m_bPaused = bPause;
}