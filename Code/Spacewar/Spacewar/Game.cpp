#include <thread>

#include <SFML/Window/Event.hpp>

#include "Game.h"
#include "RenderSystem.h"
#include "PhysicalSystem.h"
#include "LogicalSystem.h"
#include "RenderProxy.h"

CGame::CGame()
	: m_pLogicalSystem(std::make_unique<CLogicalSystem>())
	, m_pPhysicalSystem(std::make_unique<CPhysicalSystem>())
	, m_pRenderSystem(std::make_unique<CRenderSystem>())
	, m_pRenderProxy(std::make_unique<CRenderProxy>())
{
	m_window.create(sf::VideoMode(800, 800), "Spacewar");
	m_window.setActive(false);
	m_view.reset(sf::FloatRect(0.f, 0.f, 800.f, 800.f));
}

CGame::~CGame() = default;

static constexpr const float garbage_collection_inverval = 10.f;

void CGame::Start()
{
	std::thread render(StartRender);

	sf::Clock gcClock;
	sf::Clock frameClock;


	m_pLogicalSystem->CreateTestObject();



	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

		m_pPhysicalSystem->ProcessCollisions();
		m_pLogicalSystem->Update(frameClock.getElapsedTime());

		bool bCollectGarbage = gcClock.getElapsedTime().asSeconds() >= garbage_collection_inverval;

		{
			std::lock_guard<std::mutex> lock(m_renderLock);
			
			if (bCollectGarbage)
			{
				// processing garbage within the time interval?
				m_pRenderSystem->CollectGarbage();
			}
			m_pRenderProxy->SwitchStreams();
			// pass rendering entities count
		}

		if (bCollectGarbage)
		{
			m_pLogicalSystem->CollectGarbage();
			m_pPhysicalSystem->CollectGarbage();
			
			gcClock.restart();
		}

		frameClock.restart();
	}

	render.join();
}

void CGame::StartRender()
{
	CGame& game = CGame::Get();

	game.m_window.setActive(true);

	while (game.m_window.isOpen())
	{
		{
			std::lock_guard<std::mutex> lock(game.m_renderLock);
			
			game.m_pRenderProxy->ExecuteCommands();
		}

		game.m_window.clear();
		game.m_pRenderSystem->Render(game.m_window);
		game.m_window.display();
	}
}